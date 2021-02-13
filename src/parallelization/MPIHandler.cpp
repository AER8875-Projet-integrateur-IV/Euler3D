#include <parallelization/MPIHandler.hpp>


using namespace E3D::Parallel;

MPIHandler::MPIHandler(int argc, char* argv[]) {

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&_rankID);
    MPI_Comm_size(MPI_COMM_WORLD,&_poolSize);



}
