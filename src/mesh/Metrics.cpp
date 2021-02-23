//
// Created by amin on 2/21/21.
//

#include "mesh/Metrics.hpp"

using namespace E3D;
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
	if (e3d_mpi.getRankID() == 0) {
		printf("Computing Metrics took %.5f seconds .\n", endMetricsTimer - startMetricsTimer);
	}
}

void Metrics::computeFaceMetrics() {
	const int nFaces = _localMesh.GetnFace();
	//Temporary Variables

	int temp_nNodesSurrFace;                                    // Number of nodes surrounding face
	std::vector<Vector3<double>> temp_LocalNodesCoords;         // Hold looped face Node Coordinates
	double temp_area;                                           // Hold area of the looped face
	Vector3<double> temp_centroid;                              // hold centroid of looped face
	Vector3<double> temp_Normal;                                // Hold normal vector of looped face

	//Iterate over all faces
	for (int iface = 0; iface < nFaces; iface++) {

		// Clear node coordinates for every new face
		temp_LocalNodesCoords.clear();

		// Search for nodes connected
		int* p_NodeID = _localMesh.GetFace2NodeID(iface,temp_nNodesSurrFace);

		// Reserve depending on number of nodes surrounding face
		temp_LocalNodesCoords.reserve(temp_nNodesSurrFace);

		// populate LocalNodesCoords variable
		for(int localNodeID=0;localNodeID<temp_nNodesSurrFace;localNodeID++){

			E3D::Parser::Node temp_coords = _localMesh.GetNodeCoord(p_NodeID[localNodeID]);
			temp_LocalNodesCoords.emplace_back(temp_coords.getX(),temp_coords.getY(),temp_coords.getZ());
		}

		// if triangle
		if (temp_nNodesSurrFace == 3){

			//Compute face center
            temp_centroid = (temp_LocalNodesCoords[0] + temp_LocalNodesCoords[1] + temp_LocalNodesCoords[2]) * 0.3333333 ;// Multiplication cheaper than divison

			// Compute face Area
			// TODO Change formulation from 2D TO 3D
			temp_area =  0.5 * std::abs((temp_LocalNodesCoords[0].x - temp_LocalNodesCoords[1].x) * (temp_LocalNodesCoords[0].y + temp_LocalNodesCoords[1].y) +
                                        (temp_LocalNodesCoords[1].x - temp_LocalNodesCoords[2].x) * (temp_LocalNodesCoords[1].y + temp_LocalNodesCoords[2].y) +
                                        (temp_LocalNodesCoords[2].x - temp_LocalNodesCoords[0].x) * (temp_LocalNodesCoords[2].y + temp_LocalNodesCoords[0].y));

			// compute face normal vector



		}

		// if Quad
		else if (temp_nNodesSurrFace == 4){

			// Compute face center
            temp_centroid = (temp_LocalNodesCoords[0] + temp_LocalNodesCoords[1] + temp_LocalNodesCoords[2] + temp_LocalNodesCoords[4]) * 0.25 ;

			// Compute face Area

			//Compute face normal Vector
		}

	}
}

void Metrics::computeCellMetrics() {
	const int nElem = _localMesh.GetMeshInteriorElemCount();

	//Iterate over all Volume Elements
	for (int iElem = 0; iElem < nElem; iElem++) {

		// Search for nodes connected

		// Compute cell Centroid

		// Compute Cell Volume
	}
}