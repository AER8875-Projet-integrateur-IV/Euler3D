//
// Created by amin on 2/21/21.
//

#include "mesh/Metrics.hpp"


E3D::Metrics::Metrics(const Mesh<Parser::MeshPartition> &localMesh, const Parallel::MPIHandler &e3d_mpi)
    : _localMesh(localMesh) {

	if (e3d_mpi.getRankID() == 0) {
		std::cout << "\n\n"
		          << std::string(24, '#') << "  Geometrical Quantites  " << std::string(24, '#') << "\n\n";
	}
	double startMetricsTimer = MPI_Wtime();

	int nElem = _localMesh.GetMeshInteriorElemCount();

	//Reserve space to avoid copying
	_faceCenters.reserve(nElem);
	_faceNormals.reserve(nElem);
	_cellCentroids.reserve(nElem);
	_faceSurfaces.reserve(nElem);
	_cellVolumes.reserve(nElem);


	double endMetricsTimer = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);
	if(e3d_mpi.getRankID() == 0) {
        printf("Computing Metrics took %.5f seconds .\n", endMetricsTimer-startMetricsTimer);
	}
}