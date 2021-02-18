
#include <cstdlib>
#include <iostream>
#include <mpi.h>

#include "mesh/Mesh.hpp"
#include "parallelization/MPIHandler.hpp"
#include "parser/SimConfig.hpp"
using namespace E3D;

int main(int argc, char *argv[]) {

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

	E3D::Mesh<E3D::Parser::MeshPartition> localmesh(config.getPartitionedMeshFiles()[e3d_mpi.getRankID()], e3d_mpi);


	// Parsing Partitions (mesh files)
	double startConnectivityTimer = MPI_Wtime();
	localmesh.solveConnectivity();
	MPI_Barrier(MPI_COMM_WORLD);
	if (e3d_mpi.getRankID() == 0) {
		double endConnectivityTimer = MPI_Wtime();

        printf("Connectivity took %.5f seconds to solve.\n", endConnectivityTimer-startConnectivityTimer);
	}


	e3d_mpi.finalize();


	return 0;
}