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
//    if (e3d_mpi.getRankID() == 0) {
//
//        printf("Total Domain Volume : %.3f \n", domainVolume);
//        printf("Computing Metrics took %.5f seconds .\n", endMetricsTimer - startMetricsTimer);
//
//    }


}

void Solver::EulerSolver::Run() {

	E3D::Solver::printHeader();


	while(_maximumLocalRms > _config.getMinResidual() && _nbInteration < _config.getMaxNumberIterations()){

		double iterationBeginimer = MPI_Wtime();
        updateBC();
		_nbInteration += 1;

        if (_e3d_mpi.getRankID() == 0) {
			double iterationEndTimer = MPI_Wtime();
			double iterationwallTime = iterationEndTimer - iterationBeginimer;
            E3D::Solver::PrintSolverIteration(_CL,_CD,_maximumLocalRms,iterationwallTime,_nbInteration);
        }
        MPI_Barrier(MPI_COMM_WORLD);




	}

	if(_config.getMaxNumberIterations() == _nbInteration){
        if (_e3d_mpi.getRankID() == 0) {
            std::cout << "Solver exceeded maximum number of iterations";
        }
	}

}

void Solver::EulerSolver::updateBC() {

}