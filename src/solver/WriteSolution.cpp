#include "solver/WriteSolution.hpp"

using namespace E3D::Solver;

WriteSolution::WriteSolution(E3D::Solver::FlowField &flowField,
                             const E3D::Mesh<E3D::Parser::MeshPartition> &localMesh,
                             const E3D::Parser::SimConfig &config,
                             const E3D::Parallel::MPIHandler &e3d_mpi) : _flowField(flowField), _localMesh(localMesh) {
	_fileName = config.getPartitionedMeshFiles()[e3d_mpi.getRankID()];
	_Solfile = std::ofstream(_fileName + ".sol");
	_Solfile << std::setiosflags(std::ios::dec)
	         << std::setiosflags(std::ios::scientific);
	_Boundaryfile = std::ofstream(_fileName + ".bry");
	write();
	return;
}

WriteSolution::~WriteSolution() {
	_Solfile.close();
	return;
}

void WriteSolution::write() {
	writeSolutionFile();
	writeBoundaryFile();
}


void WriteSolution::writeSolutionFile() {
	_Solfile << std::setw(20) << "density"
	         << std::setw(20) << "velocity_x"
	         << std::setw(20) << "velocity_y"
	         << std::setw(20) << "velocity_z"
	         << std::setw(20) << "pressure"
	         << std::setw(20) << "energy"
	         << std::endl;

	std::vector<double> rho = _flowField.Getrho();
	std::vector<double> u = _flowField.GetU_Velocity();
	std::vector<double> v = _flowField.GetV_Velocity();
	std::vector<double> w = _flowField.GetW_Velocity();
	std::vector<double> p = _flowField.GetP();
	std::vector<double> E = _flowField.GetE();

	for (size_t i = 0; i < rho.size(); i++) {
		_Solfile << std::setw(20) << rho[i]
		         << std::setw(20) << u[i]
		         << std::setw(20) << v[i]
		         << std::setw(20) << w[i]
		         << std::setw(20) << p[i]
		         << std::setw(20) << E[i]
		         << std::endl;
	}
}

void WriteSolution::writeBoundaryFile() {
	std::vector<int> WallGhostCellIDs = _localMesh.GetWallGhostCellsIDs();
	std::vector<int> WallAdjacentToGhostCellIDs = _localMesh.GetWallAdjacentGhostCellIDs();
	std::vector<int> WallAdjacentFaceIDs = _localMesh.GetWallAdjacentFaceIDs();
	_Boundaryfile << "NOMBRE DE CELLULES= " << WallGhostCellIDs.size() << std::endl;
	for (size_t GhostID = 0; GhostID < WallGhostCellIDs.size(); GhostID++) {
		_Boundaryfile << std::setw(9) << WallAdjacentToGhostCellIDs[GhostID];
		int FaceGhostIdx = WallAdjacentFaceIDs[GhostID];
		int nNode;
		int *ptr = _localMesh.GetFace2NodeID(FaceGhostIdx, nNode);
		_Boundaryfile << std::setw(9) << nNode;
		for (int iNode = 0; iNode < nNode; iNode++) {
			_Boundaryfile << std::setw(9) << ptr[iNode];
		}
		_Boundaryfile << std::endl;
	}
}