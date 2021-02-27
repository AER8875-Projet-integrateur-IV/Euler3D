//
// Created by amin on 2/21/21.
//
#include "mesh/Metrics.hpp"
#include <numeric>
#include <cmath>

using namespace E3D;
E3D::Metrics::Metrics(const Mesh<Parser::MeshPartition> &localMesh, const Parallel::MPIHandler &e3d_mpi)
    : _localMesh(localMesh) {

	if (e3d_mpi.getRankID() == 0) {
		std::cout << "\n\n"
		          << std::string(24, '#') << "  Geometrical Quantities  " << std::string(24, '#') << "\n\n";
	}

	double startMetricsTimer = MPI_Wtime();

	int nElem = _localMesh.GetMeshInteriorElemCount();
	int nFace = _localMesh.GetnFace();

	//Reserve space to avoid copying
	_faceCenters.reserve(nFace);
	_faceNormals.reserve(nFace);
	_cellCentroids.reserve(nElem);
	_faceSurfaces.reserve(nFace);
	_cellVolumes.reserve(nElem);

	computeFaceMetrics();
	computeCellMetrics();

	double endMetricsTimer = MPI_Wtime();


    double domainVolume=0;

    double PartitionVolume = std::accumulate(_cellVolumes.begin(),_cellVolumes.end(),0.0);
    MPI_Reduce(&PartitionVolume,&domainVolume,1,MPI_DOUBLE_PRECISION,MPI_SUM,0,MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	if (e3d_mpi.getRankID() == 0) {
        printf("Total Domain Volume : %.3f \n", domainVolume);
		printf("Computing Metrics took %.5f seconds .\n", endMetricsTimer - startMetricsTimer);

	}
}

// Compute area of a triangle with given to vectors
double computeTriangleArea(Vector3<double> A, Vector3<double> B) {
	double lengthProduct = A.length() * B.length();
	double sinTheta = std::sqrt(1 - std::pow(Vector3<double>::dot(A, B) / lengthProduct, 2));
	return 0.5 * (lengthProduct) *sinTheta;
}


void Metrics::computeFaceMetrics() {
	const int nFaces = _localMesh.GetnFace();
	//Temporary Variables

	int temp_nNodesSurrFace;                           // Number of nodes surrounding face
	std::vector<Vector3<double>> temp_LocalNodesCoords;// Hold looped face Node Coordinates
	double temp_area;                                  // Hold area of the looped face
	Vector3<double> temp_centroid;                     // hold centroid of looped face
	Vector3<double> temp_Normal;                       // Hold normal vector of looped face
	Vector3<double> temp_unitVector;

	//Iterate over all faces
	for (int iface = 0; iface < nFaces; iface++) {

		// Clear node coordinates for every new face
		temp_LocalNodesCoords.clear();

		// Search for nodes connected
		int *p_NodeID = _localMesh.GetFace2NodeID(iface, temp_nNodesSurrFace);

		// Reserve depending on number of nodes surrounding face
		temp_LocalNodesCoords.reserve(temp_nNodesSurrFace);

		// populate LocalNodesCoords variable
		for (int localNodeID = 0; localNodeID < temp_nNodesSurrFace; localNodeID++) {

			E3D::Parser::Node temp_coords = _localMesh.GetNodeCoord(p_NodeID[localNodeID]);
			temp_LocalNodesCoords.emplace_back(temp_coords.getX(), temp_coords.getY(), temp_coords.getZ());
		}

		// if triangle
		if (temp_nNodesSurrFace == 3) {

			//Compute face center
			temp_centroid = (temp_LocalNodesCoords[0] + temp_LocalNodesCoords[1] + temp_LocalNodesCoords[2]) * 0.33333333;// Multiplication cheaper than divison

			// Compute face Area

			Vector3<double> AB = temp_LocalNodesCoords[1] - temp_LocalNodesCoords[0];
			Vector3<double> AC = temp_LocalNodesCoords[2] - temp_LocalNodesCoords[0];
			temp_area = computeTriangleArea(AB, AC);

			// compute face normal vector
			temp_Normal = Vector3<double>::crossProduct(AC, AB);
			temp_unitVector = temp_Normal / temp_Normal.length();


		}

		// if Quad
		else if (temp_nNodesSurrFace == 4) {

			// Compute face center
			temp_centroid = (temp_LocalNodesCoords[0] + temp_LocalNodesCoords[1] + temp_LocalNodesCoords[2] + temp_LocalNodesCoords[3]) * 0.25;

			// Compute face Area
			Vector3<double> AB = temp_LocalNodesCoords[1] - temp_LocalNodesCoords[0];
			Vector3<double> AC = temp_LocalNodesCoords[2] - temp_LocalNodesCoords[0];
			Vector3<double> AD = temp_LocalNodesCoords[3] - temp_LocalNodesCoords[0];

			temp_area = computeTriangleArea(AB, AC) + computeTriangleArea(AC, AD);

			//Compute face normal Vector
			temp_Normal = Vector3<double>::crossProduct(AB, AC);// Normal of one of both triangles == Normal of the quad (if planar surface)
			temp_unitVector = temp_Normal / temp_Normal.length();
		}
		_faceSurfaces.push_back(temp_area);
		_faceCenters.push_back(temp_centroid);
		_faceNormals.push_back(temp_Normal);
		_faceUnitNormals.push_back(temp_unitVector);
	}
}

void Metrics::computeCellMetrics() {
	const int nElem = _localMesh.GetMeshInteriorElemCount();

	std::vector<Vector3<double>> temp_LocalNodesCoords;// Hold looped face Node Coordinates
	Vector3<double> temp_centroid;                     // hold centroid of looped Cell


	//Iterate over all Volume Elements
	for (int iElem = 0; iElem < nElem; iElem++) {

		// Search for nodes connected

		// Clear node coordinates for every new face
		temp_LocalNodesCoords.clear();

		// Search for nodes connected
		std::vector<int> temp_localNodes = _localMesh.GetInteriorElement(iElem).getElemNodes();

		temp_LocalNodesCoords.reserve(temp_localNodes.size());

		// populate LocalNodesCoords variable
		for (const auto &nodeID : temp_localNodes) {

			E3D::Parser::Node temp_coords = _localMesh.GetNodeCoord(nodeID);
			temp_LocalNodesCoords.emplace_back(temp_coords.getX(), temp_coords.getY(), temp_coords.getZ());
		}
		int numberOfConnectedFaces = 0;
		int *p_localFaces = _localMesh.GetElement2FaceID(iElem, numberOfConnectedFaces);

		// temporary values for volume and centroids computation


		Vector3<double> centroid_numerator(0, 0, 0);// Formulas for the computation of faces and centroids
		double centroid_denominator = 0;            // are taken from Blazek's CFD Principles and Applications Pages 129 and 130
		double temp_volume = 0;

		Vector3<double> cellCenter = std::accumulate(temp_LocalNodesCoords.begin(),
		                                             temp_LocalNodesCoords.end(),
		                                             Vector3<double>(0, 0, 0)) / temp_LocalNodesCoords.size();


		for (int ifaceLocal = 0; ifaceLocal < numberOfConnectedFaces; ifaceLocal++) {
			// Prepare temporary vectors for computation
			Vector3<double> faceNormalVector = _faceNormals[p_localFaces[ifaceLocal]];
			Vector3<double> faceCenter = _faceCenters[p_localFaces[ifaceLocal]];
			Vector3<double> faceUnitVector = _faceUnitNormals[p_localFaces[ifaceLocal]];
			Vector3<double> faceToCellCenter = faceCenter - cellCenter;
            double surfaceLength = faceNormalVector.length();

            // Check if cell normal vector is pointing outward the element
			// Cos(theta) between normal face vector and CellCenter-to-facecenter is computed
			// if cos(theta) < 0 invert face Normal direction
            double CosthetaCellCenterFace = Vector3<double>::dot(faceToCellCenter, faceNormalVector) / (faceToCellCenter.length() * faceNormalVector.length());
            if (CosthetaCellCenterFace < 0) {
                faceNormalVector *= -1;
                faceUnitVector *= -1;
            }

			// Compute Centroid
            centroid_numerator += (faceCenter * Vector3<double>::dot(faceCenter, faceUnitVector) * surfaceLength) * 3;
            centroid_denominator += (Vector3<double>::dot(faceCenter, faceUnitVector) * surfaceLength) * 4;



            // Compute Cell Volume
			temp_volume += Vector3<double>::dot(faceCenter, faceNormalVector) * 0.33333333;

		}

		temp_centroid = centroid_numerator / centroid_denominator;

		_cellCentroids.push_back(temp_centroid);
		_cellVolumes.push_back(temp_volume);
	}
}