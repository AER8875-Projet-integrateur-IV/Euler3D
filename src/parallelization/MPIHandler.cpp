#include <algorithm>
#include <parallelization/MPIHandler.hpp>
#include <set>
using namespace E3D::Parallel;

MPIHandler::MPIHandler(int argc, char *argv[]) {

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankID);
	MPI_Comm_size(MPI_COMM_WORLD, &_poolSize);
	if (_rankID == 0) printf("\nNumber of MPI processes launched : %d\n", _poolSize);
}


void MPIHandler::sortInterface() {
    // Sort requester ID
    for (auto &[tagID, VectorGhostCell] : _requesterID) {
        std::sort(VectorGhostCell.begin(), VectorGhostCell.end());
    }
    // Sort Sender ID
    for (auto &[tagID, VectorGhostCell] : _senderID) {
        std::sort(VectorGhostCell.begin(), VectorGhostCell.end(),
                  [](E3D::Parser::GhostCell a, E3D::Parser::GhostCell b) { return a.getthisPartitionElementID() < b.getthisPartitionElementID(); });
    }

    // Following code creates unique pairs of partition for whole field
    // and populates member variable _UniquePairs
    std::vector<int> possible_combination;
    possible_combination.reserve(_requesterID.size() * 2);
    int num_combinations = _requesterID.size();
    for (auto &[PartitionID, GhostCells] : _requesterID) {
        possible_combination.push_back(_rankID);
        possible_combination.push_back(PartitionID);
    }
    std::vector<int> Ncombinations_per_partition;
    Ncombinations_per_partition.resize(_poolSize);

    MPI_Gather(&num_combinations, 1, MPI_INT, &Ncombinations_per_partition[0], 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 1; i < _poolSize; i++) {
        if (_rankID == i) {
            MPI_Send(&possible_combination[0], _requesterID.size() * 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        if (_rankID == 0) {
            std::vector<int> temp_combinations;
            temp_combinations.resize(Ncombinations_per_partition[i] * 2);
            MPI_Recv(&temp_combinations[0], Ncombinations_per_partition[i] * 2, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (auto &value : temp_combinations) {
                possible_combination.push_back(value);
            }
        }
    }

    if (_rankID == 0) {
        std::set<std::pair<int, int>> listOfPairs;
        for (size_t i = 0; i < possible_combination.size(); i += 2) {
            if (possible_combination[i] > possible_combination[i + 1]) {
                std::swap(possible_combination[i], possible_combination[i + 1]);
            }
            listOfPairs.insert(std::make_pair(possible_combination[i], possible_combination[i + 1]));
        }

        _uniquePairs.reserve(listOfPairs.size() * 2);
        for (auto [int1, int2] : listOfPairs) {
            _uniquePairs.push_back(int1);
            _uniquePairs.push_back(int2);
        }
        _uniquePairsSize = _uniquePairs.size();
    }

    MPI_Bcast(&_uniquePairsSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (_rankID != 0) {
        _uniquePairs.resize(_uniquePairsSize);
    }

    MPI_Bcast(&_uniquePairs[0], _uniquePairsSize, MPI_INT, 0, MPI_COMM_WORLD);
}

/**
 * @brief Communicate primitive variables between partition
 *  Communication is done by looping through each pair found in _uniquePairs
 */
void MPIHandler::updateFlowField(E3D::Solver::FlowField &localFlowField) const {
    MPI_Barrier(MPI_COMM_WORLD);
    for (int PairIndex = 0; PairIndex < _uniquePairsSize; PairIndex += 2) {
        if (_rankID == _uniquePairs[PairIndex] || _rankID == _uniquePairs[PairIndex + 1]) {
            if (_rankID == _uniquePairs[PairIndex]) {
                int localPartitionPos = 0;
                for (auto &[PartitionID, VectorGhostCell] : _requesterID) {
                    if (_uniquePairs[PairIndex + 1] == PartitionID) {
                        int VecSize = VectorGhostCell.size() * 5;
                        std::vector<double> VarToSend;
                        VarToSend.reserve(VecSize);
                        // Loop through the whole Variable field at a time to be more efficient (with a cost of more lines of code)
                        for (auto &GhostCell : VectorGhostCell) {
                            VarToSend.push_back(localFlowField.Getrho()[GhostCell.getGhostCellID()]);
                        }
                        for (auto &GhostCell : VectorGhostCell) {
                            VarToSend.push_back(localFlowField.GetU_Velocity()[GhostCell.getGhostCellID()]);
                        }
                        for (auto &GhostCell : VectorGhostCell) {
                            VarToSend.push_back(localFlowField.GetV_Velocity()[GhostCell.getGhostCellID()]);
                        }
                        for (auto &GhostCell : VectorGhostCell) {
                            VarToSend.push_back(localFlowField.GetW_Velocity()[GhostCell.getGhostCellID()]);
                        }
                        for (auto &GhostCell : VectorGhostCell) {
                            VarToSend.push_back(localFlowField.GetE()[GhostCell.getGhostCellID()]);
                        }
                        // Recv Buffer, contains variables in this order : [ rho's, u's, v's, w's , E's ]
                        std::vector<double> VarToReceive;
                        VarToReceive.resize(VecSize, 0);

                        MPI_Sendrecv(&VarToSend[0],
                                     VecSize,
                                     MPI_DOUBLE, PartitionID,
                                     0,
                                     &VarToReceive[0],
                                     VecSize,
                                     MPI_DOUBLE,
                                     PartitionID,
                                     1,
                                     MPI_COMM_WORLD,
                                     MPI_STATUS_IGNORE);

                        // Update Rho
                        for (size_t i = 0; i < VectorGhostCell.size(); i++) {
                            localFlowField.setRho(_senderID[localPartitionPos].second[i % VectorGhostCell.size()].getGhostCellID(), VarToReceive[i]);
                        }
                        // Update U
                        for (size_t i = VectorGhostCell.size(); i < VectorGhostCell.size() * 2; i++) {
                            localFlowField.setU(_senderID[localPartitionPos].second[i % VectorGhostCell.size()].getGhostCellID(), VarToReceive[i]);
                        }
                        // Update V
                        for (size_t i = VectorGhostCell.size() * 2; i < VectorGhostCell.size() * 3; i++) {
                            localFlowField.setV(_senderID[localPartitionPos].second[i % VectorGhostCell.size()].getGhostCellID(), VarToReceive[i]);
                        }
                        // Update W
                        for (size_t i = VectorGhostCell.size() * 3; i < VectorGhostCell.size() * 4; i++) {
                            localFlowField.setW(_senderID[localPartitionPos].second[i % VectorGhostCell.size()].getGhostCellID(), VarToReceive[i]);
                        }
                        // Update E
                        for (size_t i = VectorGhostCell.size() * 4; i < VectorGhostCell.size() * 5; i++) {
                            localFlowField.setE(_senderID[localPartitionPos].second[i % VectorGhostCell.size()].getGhostCellID(), VarToReceive[i]);
                        }

                        // Update p
                        for (size_t i = 0; i < VectorGhostCell.size(); i++) {
                            int ghost_idx = _senderID[localPartitionPos].second[i].getGhostCellID();
                            double p = (localFlowField.getgamma_ref() - 1) * localFlowField.Getrho()[ghost_idx] * (localFlowField.GetE()[ghost_idx] - (std::pow(localFlowField.GetU_Velocity()[ghost_idx], 2) + std::pow(localFlowField.GetV_Velocity()[ghost_idx], 2) + std::pow(localFlowField.GetW_Velocity()[ghost_idx], 2)) / 2.0);

                            localFlowField.setP(ghost_idx, p);
                        }

                        // Update H
                        for (size_t i = 0; i < VectorGhostCell.size(); i++) {
                            int ghost_idx = _senderID[localPartitionPos].second[i].getGhostCellID();
                            double H = localFlowField.GetE()[ghost_idx] + (localFlowField.GetP()[ghost_idx] / localFlowField.Getrho()[ghost_idx]);
                            localFlowField.setH(ghost_idx, H);
                        }


                        // Update M
                        for (size_t i = 0; i < VectorGhostCell.size(); i++) {
                            int ghost_idx = _senderID[localPartitionPos].second[i].getGhostCellID();
                            double a = std::sqrt(localFlowField.getgamma_ref() * (localFlowField.GetP()[ghost_idx] / localFlowField.Getrho()[ghost_idx]));
                            double V = std::sqrt(std::pow(localFlowField.GetU_Velocity()[ghost_idx], 2) + std::pow(localFlowField.GetV_Velocity()[ghost_idx], 2) + std::pow(localFlowField.GetW_Velocity()[ghost_idx], 2));
                            localFlowField.setM(ghost_idx, V / a);
                        }
                        break;
                    }
                    localPartitionPos++;
                }
            } else if (_rankID == _uniquePairs[PairIndex + 1]) {
                int localPartitionPos = 0;
                for (auto &[PartitionID, VectorGhostCell] : _requesterID) {
                    if (_uniquePairs[PairIndex] == PartitionID) {

                        int VecSize = VectorGhostCell.size() * 5;
                        std::vector<double> VarToSend;
                        VarToSend.reserve(VecSize);
                        // Loop through the whole Variable field at a time to be more efficient (with a cost of more lines of code)
                        for (auto &GhostCell : VectorGhostCell) {
                            VarToSend.push_back(localFlowField.Getrho()[GhostCell.getGhostCellID()]);
                        }
                        for (auto &GhostCell : VectorGhostCell) {
                            VarToSend.push_back(localFlowField.GetU_Velocity()[GhostCell.getGhostCellID()]);
                        }
                        for (auto &GhostCell : VectorGhostCell) {
                            VarToSend.push_back(localFlowField.GetV_Velocity()[GhostCell.getGhostCellID()]);
                        }
                        for (auto &GhostCell : VectorGhostCell) {
                            VarToSend.push_back(localFlowField.GetW_Velocity()[GhostCell.getGhostCellID()]);
                        }
                        for (auto &GhostCell : VectorGhostCell) {
                            VarToSend.push_back(localFlowField.GetE()[GhostCell.getGhostCellID()]);
                        }
                        // Recv Buffer, contains variables in this order : [ rho's, u's, v's, w's , E's ]
                        std::vector<double> VarToReceive;
                        VarToReceive.resize(VecSize, 0.0);

                        MPI_Sendrecv(&VarToSend[0],
                                     VecSize,
                                     MPI_DOUBLE, PartitionID,
                                     1,
                                     &VarToReceive[0],
                                     VecSize,
                                     MPI_DOUBLE,
                                     PartitionID,
                                     0,
                                     MPI_COMM_WORLD,
                                     MPI_STATUS_IGNORE);


                        // Update Rho
                        for (size_t i = 0; i < VectorGhostCell.size(); i++) {
                            localFlowField.setRho(_senderID[localPartitionPos].second[i % VectorGhostCell.size()].getGhostCellID(), VarToReceive[i]);
                        }
                        // Update U
                        for (size_t i = VectorGhostCell.size(); i < VectorGhostCell.size() * 2; i++) {
                            localFlowField.setU(_senderID[localPartitionPos].second[i % VectorGhostCell.size()].getGhostCellID(), VarToReceive[i]);
                        }
                        // Update V
                        for (size_t i = VectorGhostCell.size() * 2; i < VectorGhostCell.size() * 3; i++) {
                            localFlowField.setV(_senderID[localPartitionPos].second[i % VectorGhostCell.size()].getGhostCellID(), VarToReceive[i]);
                        }
                        // Update W
                        for (size_t i = VectorGhostCell.size() * 3; i < VectorGhostCell.size() * 4; i++) {
                            localFlowField.setW(_senderID[localPartitionPos].second[i % VectorGhostCell.size()].getGhostCellID(), VarToReceive[i]);
                        }
                        // Update E
                        for (size_t i = VectorGhostCell.size() * 4; i < VectorGhostCell.size() * 5; i++) {
                            localFlowField.setE(_senderID[localPartitionPos].second[i % VectorGhostCell.size()].getGhostCellID(), VarToReceive[i]);
                        }

                        // Update p
                        for (size_t i = 0; i < VectorGhostCell.size(); i++) {
                            int ghost_idx = _senderID[localPartitionPos].second[i].getGhostCellID();
                            double p = (localFlowField.getgamma_ref() - 1) * localFlowField.Getrho()[ghost_idx] * (localFlowField.GetE()[ghost_idx] - (std::pow(localFlowField.GetU_Velocity()[ghost_idx], 2) + std::pow(localFlowField.GetV_Velocity()[ghost_idx], 2) + std::pow(localFlowField.GetW_Velocity()[ghost_idx], 2)) / 2.0);
                            localFlowField.setP(ghost_idx, p);
                        }


                        // Update H
                        for (size_t i = 0; i < VectorGhostCell.size(); i++) {
                            int ghost_idx = _senderID[localPartitionPos].second[i].getGhostCellID();
                            double H = localFlowField.GetE()[ghost_idx] + (localFlowField.GetP()[ghost_idx] / localFlowField.Getrho()[ghost_idx]);
                            localFlowField.setH(ghost_idx, H);
                        }


                        // Update M
                        for (size_t i = 0; i < VectorGhostCell.size(); i++) {
                            int ghost_idx = _senderID[localPartitionPos].second[i].getGhostCellID();
                            double a = std::sqrt(localFlowField.getgamma_ref() * (localFlowField.GetP()[ghost_idx] / localFlowField.Getrho()[ghost_idx]));
                            double V = std::sqrt(std::pow(localFlowField.GetU_Velocity()[ghost_idx], 2) + std::pow(localFlowField.GetV_Velocity()[ghost_idx], 2) + std::pow(localFlowField.GetW_Velocity()[ghost_idx], 2));
                            localFlowField.setM(ghost_idx, V / a);
                        }

                        break;
                    }
                    localPartitionPos++;
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

E3D::Vector3<double> MPIHandler::UpdateAerodynamicCoefficients(const E3D::Vector3<double> &aerodynamicCoeff) const {
	std::array<double, 3> coeffsSender = {aerodynamicCoeff.x,
	                                      aerodynamicCoeff.y,
	                                      aerodynamicCoeff.z};
	// std::array<double, 3> coeffsSender = {_rankID,
	//                                       _rankID,
	//                                       _rankID};
	int nCoeff = 3;
	int nTotalCoeff = _poolSize * nCoeff;
	std::vector<double> coeffsRecv(nTotalCoeff);// Could be modified to only create
	                                            // a buffer for rank 0
	// std::cout << "sent from part " << _rankID << std::endl;

	// for (int i = 0; i < coeffsSender.size(); i++)
	// 	std::cout << coeffsSender.at(i) << ' ';

	// std::cout << std::endl;
	MPI_Gather(&coeffsSender[0], nCoeff, MPI_DOUBLE,
	           &coeffsRecv[0], nCoeff, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	// Ranks other than 0 do not have to update their coefficients
	if (_rankID == 0) {
		// std::cout << "coeffsRecv:" << std::endl;
		// for (int i = 0; i < coeffsRecv.size(); i++)
		// 	std::cout << coeffsRecv.at(i) << ' ';
		// std::cout << std::endl;
		int bufferIndex = 3;
		// std::cout << _poolSize << std::endl;
		for (int i = 1; i < _poolSize; i++) {
			// CL
			coeffsSender[0] += coeffsRecv[bufferIndex];
			bufferIndex++;
			// CD
			coeffsSender[1] += coeffsRecv[bufferIndex];
			bufferIndex++;
			//
			coeffsSender[2] += coeffsRecv[bufferIndex];
			bufferIndex++;
		}
		// std::cout << "coeff:" << std::endl;
		// for (int i = 0; i < 3; i++)
		// 	std::cout << coeffsSender.at(i) << ' ';
		// std::cout << std::endl;
	}
	return E3D::Vector3<double>(coeffsSender[0], coeffsSender[1], coeffsSender[2]);
}