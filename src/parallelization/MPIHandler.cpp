#include <algorithm>
#include <parallelization/MPIHandler.hpp>

using namespace E3D::Parallel;

MPIHandler::MPIHandler(int argc, char *argv[]) {

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankID);
	MPI_Comm_size(MPI_COMM_WORLD, &_poolSize);
	if (_rankID == 0) printf("\nNumber of MPI processes launched : %d\n", _poolSize);
}


void MPIHandler::sortInterfaceRequester() {

	for (auto &[tagID, VectorGhostCell] : _requesterID) {
		std::sort(VectorGhostCell.begin(), VectorGhostCell.end());
	}

	for (auto &[tagID, VectorGhostCell] : _senderID) {
		std::sort(VectorGhostCell.begin(), VectorGhostCell.end(),
		          [](E3D::Parser::GhostCell a, E3D::Parser::GhostCell b) { return a.getthisPartitionElementID() < b.getthisPartitionElementID(); });
	}

	std::sort(_requesterID.begin(),_requesterID.end(),[](std::pair<int,std::vector<E3D::Parser::GhostCell>> a ,std::pair<int,std::vector<E3D::Parser::GhostCell>> b){return a.first < b.first;});
    std::sort(_senderID.begin(),_senderID.end(),[](std::pair<int,std::vector<E3D::Parser::GhostCell>> a ,std::pair<int,std::vector<E3D::Parser::GhostCell>> b){return a.first < b.first;});
}

/**
 *
 * @param primitiveVarVector send vector of <rho, u, v, w ,E >
 * @return
 */
void MPIHandler::updateFlowField(E3D::Solver::FlowField &localFlowField) const {

	updateRho(localFlowField.Getrho());
//	MPI_Request requests[_requesterID.size() * 2];
//	MPI_Status status;
//	int request_num = 0;
//	s
//
//	for (auto &[PartitionID, VectorGhostCell] : _requesterID) {
//		std::vector<double> VariablesToSend;
//		VariablesToSend.resize(VectorGhostCell.size() * 5, 0);
//		for (auto &primitiveVar : primitiveVarVector) {
//			for (auto &GhostCell : VectorGhostCell) {
//				VariablesToSend.push_back(primitiveVar[GhostCell.getGhostCellID()]);
//			}
//		}
//		if (getRankID() == 0) {
//			std::cout << "Partition ID : " << PartitionID << "\n";
//		}
//		MPI_Send(&VariablesToSend.front(), VectorGhostCell.size() * 5, MPI_DOUBLE, PartitionID, PartitionID, MPI_COMM_WORLD);
//		request_num++;
//	}
//	MPI_Barrier(MPI_COMM_WORLD);
//
//	std::vector<std::vector<double>> VariablesToReceive;
//
//	for (auto &[PartitionID, VectorGhostCell] : _senderID) {
//
//		std::vector<double> VariablesToReceivePerPartition;
//		VariablesToReceivePerPartition.resize(VectorGhostCell.size() * 5, 0);
//		MPI_Recv(&VariablesToReceivePerPartition.front(), VectorGhostCell.size() * 5, MPI_DOUBLE, PartitionID, PartitionID, MPI_COMM_WORLD, &status);
//		VariablesToReceive.push_back(VariablesToReceivePerPartition);
//		request_num++;
//	}
//	std::cout << "ahhahahah"
//	          << "\n";
//	//
//	//	MPI_Waitall(_requesterID.size() * 2, requests, MPI_STATUSES_IGNORE);
//	std::cout << "hihihi"
//	          << "\n";
//	int PartitionNum = 0;
//	for (auto &[PartitionID, VectorGhostCell] : _senderID) {
//		for (size_t i = 0; i < VectorGhostCell.size(); i++) {
//			localFlowField.setRho(VectorGhostCell[i].getGhostCellID(), VariablesToReceive[PartitionNum][i]);
//		}
//		for (size_t i = VectorGhostCell.size(); i < VectorGhostCell.size() * 2; i++) {
//			localFlowField.setU(VectorGhostCell[i].getGhostCellID(), VariablesToReceive[PartitionNum][i]);
//		}
//		for (size_t i = VectorGhostCell.size() * 2; i < VectorGhostCell.size() * 3; i++) {
//			localFlowField.setV(VectorGhostCell[i].getGhostCellID(), VariablesToReceive[PartitionNum][i]);
//		}
//		for (size_t i = VectorGhostCell.size() * 3; i < VectorGhostCell.size() * 4; i++) {
//			localFlowField.setW(VectorGhostCell[i].getGhostCellID(), VariablesToReceive[PartitionNum][i]);
//		}
//		for (size_t i = VectorGhostCell.size() * 4; i < VectorGhostCell.size() * 5; i++) {
//			localFlowField.setE(VectorGhostCell[i].getGhostCellID(), VariablesToReceive[PartitionNum][i]);
//		}
//
//		PartitionNum++;
//	}

}
void MPIHandler::updateRho(std::vector<double>& localRho) const {


    int iter=0;
    int req_num=0;
    MPI_Request requests[_requesterID.size()*2];
    for (auto &[PartitionID, VectorGhostCell] : _requesterID){


		printf("Partition ID : %d \n",PartitionID);
        std::vector<double> RhoToSend;
		RhoToSend.reserve(VectorGhostCell.size());

		for (auto& GhostCell : VectorGhostCell){
			RhoToSend.push_back(localRho[GhostCell.getGhostCellID()]);
		}
        printf("Process %d sending...\n", getRankID());

		MPI_Isend(&RhoToSend.front(),VectorGhostCell.size(),MPI_DOUBLE,PartitionID,RHO_TAG,MPI_COMM_WORLD,&requests[req_num]);

		req_num++;

        std::vector<double> RhoToReceive;
        RhoToReceive.resize(VectorGhostCell.size(),0);
        MPI_Irecv(&RhoToReceive.front(),VectorGhostCell.size(),MPI_DOUBLE,PartitionID,RHO_TAG,MPI_COMM_WORLD,&requests[req_num]);
        req_num++;

//		for(size_t i=0; i< VectorGhostCell.size(); i++){
//            localRho[_senderID[iter].second[i].getGhostCellID()] = RhoToReceive[i];
//		}
        iter++;
	}
    MPI_Waitall(_requesterID.size()*2,requests,MPI_STATUSES_IGNORE);

	printf("Finished\n");

}

void MPIHandler::updateU(std::vector<double>& localU) const {

}

void MPIHandler::updateV(std::vector<double>& localV) const {

}
void MPIHandler::updateW(std::vector<double>& localW) const {

}

void MPIHandler::updateE(std::vector<double>& localE) const {

}
