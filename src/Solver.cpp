
#include <iostream>
#include <mpi.h>
#include <cstdlib>

#include "parallelization/MPIHandler.hpp"
#include "parser/MeshPartition.hpp"
#include "parser/SimConfig.hpp"

using namespace E3D;

int main(int argc, char *argv[]) {

    E3D::Parallel::MPIHandler e3d_mpi(argc,argv);

    if (e3d_mpi.getRankID() == 0) {
        std::cout << "\n\nEuler 3D Solver." << std::endl;

        if (argc != 2) {
            std::cerr << "Usage : E3D_Solver <configFile.e3d> " << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // Parsing Config file
    std::string configFile = argv[1];
    Parser::SimConfig config(configFile);

    // Check if MPI PROCESSES = Nb of partitions
    if (e3d_mpi.getRankID() == 0) {
        if (config.getNumberPartitions() != e3d_mpi.getPoolSize()) {
            printf("Number of mesh files and MPI processes are not equal !\n");
            printf("Number of partition = %i | Number of MPI processes : %i \n", config.getNumberPartitions(),
                   e3d_mpi.getPoolSize());
            exit(EXIT_FAILURE);
        }
        config.printInfo();
    }

    Parser::MeshPartition localmesh(config.getPartitionedMeshFiles()[e3d_mpi.getRankID()]);

    MPI_Barrier(MPI_COMM_WORLD);
    if (e3d_mpi.getRankID() == 0) {
        printf("All processes (#%d) parsed mesh files !\n", e3d_mpi.getPoolSize());
    }
    localmesh.printAllPartitionsInfo(e3d_mpi.getRankID());

    MPI_Barrier(MPI_COMM_WORLD);


    //Parser::MeshPartition rankPartitionParser(config.getPartitionedMeshFiles()[rankID], rankID);

    MPI_Finalize();


    return 0;
}