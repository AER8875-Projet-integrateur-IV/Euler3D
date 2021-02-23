
#include <iostream>
#include <mpi.h>
#include <cstdlib>

#include "parallelization/MPIHandler.hpp"
#include "parser/MeshPartition.hpp"
#include "parser/SimConfig.hpp"
#include "solver/FlowField.hpp"
#include "solver/SolverLoop.hpp"

using namespace E3D;

int main(int argc, char *argv[]) {

    E3D::Parallel::MPIHandler e3d_mpi(argc,argv);

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

    // Parsing Partitions (mesh files)
    Parser::MeshPartition localmesh(config.getPartitionedMeshFiles()[e3d_mpi.getRankID()], e3d_mpi);

    localmesh.printAllPartitionsInfo();

    MPI_Barrier(MPI_COMM_WORLD);


    e3d_mpi.finalize();

    // Solver related features
    Solver::FlowField SimResults(config);
    SimResults.PrintTest();

    Solver::SolverLoop MainLoop(SimResults);
    // MainLoop.Run();

    return 0;
}
