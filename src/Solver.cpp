
#include <iostream>
#include <mpi.h>
#include <cstdlib>

#include "parser/SU2PartitionParser.hpp"
#include "parser/SimConfig.hpp"

using namespace E3D;

int main(int argc, char *argv[]) {
    int rankID, nb_processes;

    // Initialize MPI code
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rankID);
    MPI_Comm_size(MPI_COMM_WORLD, &nb_processes);

    if (rankID == 0) {
        std::cout << "\n\nEuler 3D Solver." << std::endl;

        if (argc != 2) {
            std::cerr << "Usage : E3D_Solver <configFile.e3d> " << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);


    // Parsing Config file
    std::string configFile = argv[1];
    E3D::Parser::SimConfig config(configFile);

    // Check if MPI PROCESSES = Nb of partitions
    if (rankID == 0) {
        if (config.getNumberPartitions() != nb_processes) {
            printf("Number of mesh files and MPI processes are not equal !\n");
            printf("Number of partition = %i | Number of MPI processes : %i \n", config.getNumberPartitions(),
                   nb_processes);
            exit(EXIT_FAILURE);
        }
        config.printInfo();
    }

//Parser::SU2PartitionParser rankPartitionParser(config.getPartitionedMeshFiles()[rankID], rankID);

    MPI_Finalize();


    return 0;
}