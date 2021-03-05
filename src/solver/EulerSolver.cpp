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



        _nbInteration += 1;


		if (_e3d_mpi.getRankID() == 0) {
			double iterationEndTimer = MPI_Wtime();
			double iterationwallTime = iterationEndTimer - iterationBeginimer;
			E3D::Solver::PrintSolverIteration(_CL, _CD, _maximumLocalRms, iterationwallTime, _nbInteration);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}



	// If loop exited because of reaching maximum number of iteration, print this message
	if (_config.getMaxNumberIterations() >= _nbInteration) {
		if (_e3d_mpi.getRankID() == 0) {
			std::cout << "Solution not converged :  exceeded maximum number of iterations !";
		}
	}
}




// Update Farfield, Wall, Symmetry and MPI ghost cell primitive values
void Solver::EulerSolver::updateBC() {

	// Update Farfield
    for (size_t GhostID=0; GhostID < _FarfieldGhostCellIDs.size(); GhostID++) {

    }
	// Update Wall
    for (size_t GhostID=0; GhostID < _WallGhostCellIDs.size(); GhostID++) {

    }

	// Update Symmetry
    for (size_t GhostID=0; GhostID < _SymmetryGhostCellIDs.size(); GhostID++) {

    }

	// Update MPI
    _e3d_mpi.updateFlowField(_localFlowField);
}