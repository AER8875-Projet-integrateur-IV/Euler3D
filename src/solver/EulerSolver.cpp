//
// Created by amin on 3/1/21.
//

#include "solver/EulerSolver.hpp"
#include "utils/SolverPrint.hpp"


using namespace E3D;

Solver::EulerSolver::EulerSolver(FlowField &localFlowField,
                                 const E3D::Parallel::MPIHandler &e3d_mpi,
                                 const E3D::Mesh<E3D::Parser::MeshPartition> &localMesh,
                                 const E3D::Parser::SimConfig &config,
                                 const E3D::Metrics& localMetrics)

    : _localFlowField(localFlowField), _e3d_mpi(e3d_mpi), _localMesh(localMesh), _config(config), _localMetrics(localMetrics) {

	if (e3d_mpi.getRankID() == 0) {
		std::cout << "\n\n"
		          << std::string(24, '#') << "  Starting Solving Process !  " << std::string(24, '#') << "\n\n";
	}


}

void Solver::EulerSolver::Run() {

	if (_e3d_mpi.getRankID() == 0) {
        E3D::Solver::printHeader();
	}


	while (_maximumLocalRms > _config.getMinResidual() && _nbInteration < _config.getMaxNumberIterations()) {


        double iterationBeginimer = MPI_Wtime();

		// loop Through Ghost cells (Boundary Cells)
        updateBC();
		_e3d_mpi.updateFlowField(_localFlowField);




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

	// Update Farfield
    for (const auto& GhostCellID : _FarfieldGhostCellIDs) {


    }
}