//
// Created by amin on 3/1/21.
//

#include "solver/EulerSolver.hpp"
#include "utils/SolverPrint.hpp"


using namespace E3D;

Solver::EulerSolver::EulerSolver(FlowField &localFlowField,
                                 const E3D::Parallel::MPIHandler &e3d_mpi,
                                 const E3D::Mesh<E3D::Parser::MeshPartition> &localMesh,
                                 const E3D::Parser::SimConfig &config)
    : _localFlowField(localFlowField), _e3d_mpi(e3d_mpi), _localMesh(localMesh), _config(config) {

	if (e3d_mpi.getRankID() == 0) {
		std::cout << "\n\n"
		          << std::string(24, '#') << "  Starting Solving Process !  " << std::string(24, '#') << "\n\n";
	}

	MPI_Barrier(MPI_COMM_WORLD);
}

void Solver::EulerSolver::Run() {
	if (_e3d_mpi.getRankID() == 0) {
		E3D::Solver::printHeader();
	}

	const std::vector<int> MpiGhostCellIDs = _localMesh.GetMPIGhostCellsIDs();
	const std::vector<int> SymmetryGhostCellIDs = _localMesh.GetSymmetryGhostCellsIDs();
	const std::vector<int> FarfieldGhostCellIDs = _localMesh.GetFarfieldGhostCellsIDs();
	const std::vector<int> WallGhostCellIDs = _localMesh.GetWallGhostCellsIDs();


	while (_maximumLocalRms > _config.getMinResidual() && _nbInteration < _config.getMaxNumberIterations()) {

		// loop Through GHost cells (Boundary Cells)
		for (int GCElemID = _localMesh.GetMeshInteriorElemCount(); GCElemID < _localFlowField.getTotalElemsCount(); GCElemID++) {
			if (GCElemID) {
			}
		}


		double iterationBeginimer = MPI_Wtime();
		updateBC();
		_nbInteration += 1;

		if (_e3d_mpi.getRankID() == 0) {
			double iterationEndTimer = MPI_Wtime();
			double iterationwallTime = iterationEndTimer - iterationBeginimer;
			E3D::Solver::PrintSolverIteration(_CL, _CD, _maximumLocalRms, iterationwallTime, _nbInteration);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	if (_config.getMaxNumberIterations() == _nbInteration) {
		if (_e3d_mpi.getRankID() == 0) {
			std::cout << "Solver exceeded maximum number of iterations";
		}
	}
}

void Solver::EulerSolver::updateBC() {
}