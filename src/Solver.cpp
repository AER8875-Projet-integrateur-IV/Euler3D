
#include <cstdlib>
#include <iostream>
#include <mpi.h>

#include "mesh/Mesh.hpp"
#include "mesh/Metrics.hpp"
#include "parallelization/MPIHandler.hpp"
#include "parser/SimConfig.hpp"
#include "solver/EulerSolver.hpp"
#include "solver/FlowField.hpp"
#include "solver/WriteSolution.hpp"
#include <chrono>


using namespace E3D;


int main(int argc, char *argv[]) {

	auto begginingTime = std::chrono::steady_clock::now();

	E3D::Parallel::MPIHandler e3d_mpi(argc, argv);

	if (e3d_mpi.getRankID() == 0) {

		if (argc != 2) {
			std::cerr << "Usage : mpiexec -np <num of processes> ./E3D_Solveur <configFile.e3d> " << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);

	// Parsing Config file
	std::string configFile = argv[1];
	Parser::SimConfig config(configFile, e3d_mpi.getRankID(), e3d_mpi.getPoolSize());


	E3D::Mesh<E3D::Parser::MeshPartition> localmesh(config.getPartitionedMeshFiles()[e3d_mpi.getRankID()], e3d_mpi.getRankID());


	// Parsing Partitions (mesh files)

	localmesh.solveConnectivity();
	MPI_Barrier(MPI_COMM_WORLD);


	e3d_mpi.updateRequesterID(localmesh.getMPIelements());
	e3d_mpi.sortInterface();

	Metrics localMeshMetrics(localmesh, e3d_mpi.getRankID());
	MPI_Barrier(MPI_COMM_WORLD);

	E3D::Solver::FlowField localFlowField(config, localmesh);

	E3D::Solver::EulerSolver solver(localFlowField, e3d_mpi, localmesh, config, localMeshMetrics);

	auto preSolverTime = std::chrono::steady_clock::now();
	solver.Run();
	auto postSolverTime = std::chrono::steady_clock::now();

	E3D::Solver::WriteSolution writeSolution(localFlowField, localmesh, config, e3d_mpi);

	auto finalTime = std::chrono::steady_clock::now();

	long solverTimer = (std::chrono::duration_cast<std::chrono::milliseconds>(postSolverTime - preSolverTime)).count();
	long totalTimer = (std::chrono::duration_cast<std::chrono::milliseconds>(finalTime - begginingTime)).count();

	solver.WriteSummary(solverTimer, totalTimer);

	e3d_mpi.finalize();
	return 0;
}