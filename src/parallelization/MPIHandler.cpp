#include <parallelization/MPIHandler.hpp>
#include <algorithm>

using namespace E3D::Parallel;

MPIHandler::MPIHandler(int argc, char* argv[]) {

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&_rankID);
    MPI_Comm_size(MPI_COMM_WORLD,&_poolSize);
    if(_rankID==0) printf("\nNumber of MPI processes launched : %d\n", _poolSize);


}

void MPIHandler::sortInterface() {
	for(auto& [tagID, VectorGhostCell] : _requesterID){
		std::sort(VectorGhostCell.begin(),VectorGhostCell.end());
	}
}
