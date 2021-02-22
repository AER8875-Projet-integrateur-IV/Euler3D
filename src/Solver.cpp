
#include <iostream>
#include <mpi.h>
#include <cstdlib>

#include "parallelization/MPIHandler.hpp"
#include "parser/MeshPartition.hpp"
#include "parser/SimConfig.hpp"
<<<<<<< HEAD
#include "solver/SolverLoop.hpp"

int main(int argc, char* argv[]){
=======
>>>>>>> c61d077ea18a1bc8325086394072324464a6c7a8

using namespace E3D;

int main(int argc, char *argv[]) {

    E3D::Parallel::MPIHandler e3d_mpi(argc,argv);

    if (e3d_mpi.getRankID() == 0) {

        if (argc != 2) {
            std::cerr << "Usage : mpiexec -np <num of processes> ./E3D_Solveur <configFile.e3d> " << std::endl;
            exit(EXIT_FAILURE);
        }
    }
<<<<<<< HEAD
    
    std::string configFile = argv[1];
    E3D::Parser::SimConfig config(configFile);
    config.printInfo();

    // Initialize problem
    E3D::Solver::SolverLoop simulation;
=======
    MPI_Barrier(MPI_COMM_WORLD);

    // Parsing Config file
    std::string configFile = argv[1];
    Parser::SimConfig config(configFile, e3d_mpi.getRankID(), e3d_mpi.getPoolSize());

    // Parsing Partitions (mesh files)
    Parser::MeshPartition localmesh(config.getPartitionedMeshFiles()[e3d_mpi.getRankID()], e3d_mpi);

    localmesh.printAllPartitionsInfo();

    MPI_Barrier(MPI_COMM_WORLD);


    e3d_mpi.finalize();


>>>>>>> c61d077ea18a1bc8325086394072324464a6c7a8
    return 0;
}
