//
// Created by amin on 2/21/21.
//
#include "mesh/Metrics.hpp"
#include <cmath>
#include <numeric>


using namespace E3D;
E3D::Metrics::Metrics(const Mesh<Parser::MeshPartition> &localMesh, int mpi_rank)
    : _localMesh(localMesh) {

	if (mpi_rank == 0) {
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

	reorientFaceVectors();
	double endMetricsTimer = MPI_Wtime();


	double domainVolume = 0;

	double PartitionVolume = std::accumulate(_cellVolumes.begin(), _cellVolumes.end(), 0.0);
	MPI_Reduce(&PartitionVolume, &domainVolume, 1, MPI_DOUBLE_PRECISION, MPI_SUM, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	if (mpi_rank == 0) {
		printf("Total Domain Volume : %.3f \n", domainVolume);
		printf("Computing Metrics took %.5f seconds .\n", endMetricsTimer - startMetricsTimer);
	}
}


// Compute area of a triangle with given to vectors
double computeTriangleArea(Vector3<double>& A, Vector3<double>& B) {
    return (Vector3<double>::crossProduct(A, B)*0.5).length();
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
			temp_centroid = (temp_LocalNodesCoords[0] + temp_LocalNodesCoords[1] + temp_LocalNodesCoords[2]) / 3.0;

			// Compute face Area

			Vector3<double> AB = temp_LocalNodesCoords[1] - temp_LocalNodesCoords[0];
			Vector3<double> AC = temp_LocalNodesCoords[2] - temp_LocalNodesCoords[0];
			temp_area = computeTriangleArea(AB, AC);

			// compute face normal vector

			temp_Normal= Vector3<double>::crossProduct(AB, AC)*0.5;

			temp_unitVector = temp_Normal / temp_Normal.length();

		}

		// if Quad
		else if (temp_nNodesSurrFace == 4) {

            Vector3<double> AB = temp_LocalNodesCoords[1] - temp_LocalNodesCoords[0];
            Vector3<double> AC = temp_LocalNodesCoords[2] - temp_LocalNodesCoords[0];
            Vector3<double> AD = temp_LocalNodesCoords[3] - temp_LocalNodesCoords[0];
			double omega123 = computeTriangleArea(AB,AC);
            double omega134 = computeTriangleArea(AC,AD);
            Vector3<double> Centroid123 = (temp_LocalNodesCoords[0] + temp_LocalNodesCoords[1] + temp_LocalNodesCoords[2]) / 3.0;
            Vector3<double> Centroid134 = (temp_LocalNodesCoords[0] + temp_LocalNodesCoords[2] + temp_LocalNodesCoords[3]) / 3.0;
			// Compute face center
            temp_centroid = (Centroid123*omega123 + Centroid134*omega134)/(omega123+omega134);
            // Compute face Area

            temp_area  = omega134 + omega123;
			double delXA = temp_LocalNodesCoords[3].x - temp_LocalNodesCoords[1].x;
			double delXB = temp_LocalNodesCoords[2].x - temp_LocalNodesCoords[0].x;
			double delYA = temp_LocalNodesCoords[3].y - temp_LocalNodesCoords[1].y;
			double delYB = temp_LocalNodesCoords[2].y - temp_LocalNodesCoords[0].y;
			double delZA = temp_LocalNodesCoords[3].z - temp_LocalNodesCoords[1].z;
			double delZB = temp_LocalNodesCoords[2].z - temp_LocalNodesCoords[0].z;

			//Compute face normal Vector
			temp_Normal = Vector3<double>(delZA * delYB - delYA * delZB, delXA * delZB - delZA * delXB, delYA * delXB - delXA * delYB) * 0.5;// Normal of one of both triangles == Normal of the quad (if planar surface)
			temp_unitVector = temp_Normal / temp_area;
		}
		_faceSurfaces.push_back(temp_area);
		_faceCenters.push_back(temp_centroid);
		_faceNormals.push_back(temp_Normal);
		_faceUnitNormals.push_back(temp_unitVector);
	}
}

void Metrics::computeCellMetrics() {
	// Variables for debugging
	double TotalWedgeVolume = 0;
	double TotalPyramidVolume = 0;
	double TotaltetVolume = 0;
	double TotalhexVolume = 0;


	const int nElem = _localMesh.GetMeshInteriorElemCount();

	std::vector<Vector3<double>> temp_LocalNodesCoords;// Hold looped face Node Coordinates
	Vector3<double> temp_centroid;                     // hold centroid of looped Cell
	double temp_volume;

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


		// Volume for hexahedron
		if (temp_localNodes.size() == 8) {
			Vector3<double> AB = temp_LocalNodesCoords[1] - temp_LocalNodesCoords[0];
			Vector3<double> AC = temp_LocalNodesCoords[2] - temp_LocalNodesCoords[0];
			Vector3<double> AD = temp_LocalNodesCoords[3] - temp_LocalNodesCoords[0];

			double temp_area = computeTriangleArea(AB, AC) + computeTriangleArea(AC, AD);
			double distanceBetweenFaces = (temp_LocalNodesCoords[0] - temp_LocalNodesCoords[4]).length();
			temp_volume = temp_area * distanceBetweenFaces;

			//Debugging
			TotalhexVolume += temp_volume;


			//Centroid
			double sumx = 0.0;
			double sumy = 0.0;
			double sumz = 0.0;
			for (int i = 0; i < 8; i++) {
				sumx += temp_LocalNodesCoords[i].x;
				sumy += temp_LocalNodesCoords[i].y;
				sumz += temp_LocalNodesCoords[i].z;
			}

			temp_centroid.x = sumx / 8.0;
			temp_centroid.y = sumy / 8.0;
			temp_centroid.z = sumz / 8.0;
		}


		// Wedge
		else if (temp_localNodes.size() == 6) {
			//Volume
			auto volume1 = TetrahedronVolume(temp_LocalNodesCoords[0], temp_LocalNodesCoords[1], temp_LocalNodesCoords[2], temp_LocalNodesCoords[3]);
			auto volume2 = TetrahedronVolume(temp_LocalNodesCoords[1], temp_LocalNodesCoords[4], temp_LocalNodesCoords[2], temp_LocalNodesCoords[3]);
			auto volume3 = TetrahedronVolume(temp_LocalNodesCoords[3], temp_LocalNodesCoords[5], temp_LocalNodesCoords[4], temp_LocalNodesCoords[2]);
			temp_volume = volume1 + volume2 + volume3;
			TotalWedgeVolume += temp_volume;

			//Centroid
			auto Centroid1 = TetrahedronCentroid(temp_LocalNodesCoords[0], temp_LocalNodesCoords[1], temp_LocalNodesCoords[2], temp_LocalNodesCoords[3]);
			auto Centroid2 = TetrahedronCentroid(temp_LocalNodesCoords[1], temp_LocalNodesCoords[2], temp_LocalNodesCoords[4], temp_LocalNodesCoords[3]);
			auto Centroid3 = TetrahedronCentroid(temp_LocalNodesCoords[3], temp_LocalNodesCoords[4], temp_LocalNodesCoords[5], temp_LocalNodesCoords[2]);
			temp_centroid = (Centroid1 * volume1 + Centroid2 * volume2 + Centroid3 * volume3) / temp_volume;
		}

		// Pyramid
		else if (temp_localNodes.size() == 5) {

			TotalPyramidVolume += temp_volume;

			auto volume1 = TetrahedronVolume(temp_LocalNodesCoords[0], temp_LocalNodesCoords[2], temp_LocalNodesCoords[3], temp_LocalNodesCoords[4]);
			auto volume2 = TetrahedronVolume(temp_LocalNodesCoords[0], temp_LocalNodesCoords[1], temp_LocalNodesCoords[2], temp_LocalNodesCoords[4]);
			temp_volume = volume1 + volume2;
			auto Centroid1 = TetrahedronCentroid(temp_LocalNodesCoords[0], temp_LocalNodesCoords[2], temp_LocalNodesCoords[3], temp_LocalNodesCoords[4]);
			auto Centroid2 = TetrahedronCentroid(temp_LocalNodesCoords[0], temp_LocalNodesCoords[1], temp_LocalNodesCoords[2], temp_LocalNodesCoords[4]);
			temp_centroid = (Centroid1 * volume1 + Centroid2 * volume2) / temp_volume;
		}


		//Volume and centroid for Tetrahedron
		else if (temp_LocalNodesCoords.size() == 4) {
			//Centroid
			Vector3<double> sumNodes = {0.0, 0.0, 0.0};
			for (int i = 0; i < 4; i++) {
				sumNodes += temp_LocalNodesCoords[i];
			}
			temp_centroid = sumNodes / 4.0;


			temp_volume = TetrahedronVolume(temp_LocalNodesCoords[0], temp_LocalNodesCoords[1], temp_LocalNodesCoords[2], temp_LocalNodesCoords[3]);


			TotaltetVolume += temp_volume;
		}

		_cellCentroids.push_back(temp_centroid);

		_cellVolumes.push_back(temp_volume);
	}

	std::array<double, 4> totalvolumesToSend = {TotaltetVolume, TotalhexVolume, TotalPyramidVolume, TotalWedgeVolume};
	std::array<double, 4> totalvolumesToRcv;

	MPI_Reduce(&totalvolumesToSend[0], &totalvolumesToRcv[0], 4, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
}

void Metrics::reorientFaceVectors() {

	const int nFaces = _localMesh.GetnFace();

	for (int iface = 0; iface < nFaces; iface++) {

		int *ptr = _localMesh.GetFace2ElementID(iface);


		int elem0 = ptr[0];
		int elem1 = ptr[1];
		E3D::Vector3<double> Coord0 = _faceCenters[iface];
		E3D::Vector3<double> Coord1 = _cellCentroids[elem0];


		E3D::Vector3<double> From0to1 = Coord1 - Coord0;

		double CosthetaCellCenterFace = Vector3<double>::dot(From0to1, _faceNormals[iface]);
		// IF connected to interior cell
		if (elem1 < _localMesh.GetMeshInteriorElemCount()) {

			E3D::Vector3<double> Coord2 = _cellCentroids[elem1];
			E3D::Vector3<double> From0to2 = Coord2 - Coord0;
			double CosthetaCellCenterFace2 = Vector3<double>::dot(From0to2, _faceNormals[iface]) ;
			if (CosthetaCellCenterFace > 0 && CosthetaCellCenterFace2 < 0) {
				_faceNormals[iface] *= -1;
				_faceUnitNormals[iface] *= -1;
//                printf("Face reoriented !!!\n");
			}
		}
		// If connected to ghost cell
		else {
			if (CosthetaCellCenterFace > 0) {
//                printf("Face reoriented BC!!!\n");
				_faceNormals[iface] *= -1;
				_faceUnitNormals[iface] *= -1;
			}
		}
	}
}

double Metrics::TetrahedronVolume(Vector3<double> &pts0, Vector3<double> &pts1, Vector3<double> &pts2, Vector3<double> &pts3) {
	//Volume
	double U = (pts0 - pts1).length();
	double V = (pts1 - pts2).length();
	double W = (pts2 - pts0).length();
	double u = (pts2 - pts3).length();
	double v = (pts0 - pts3).length();
	double w = (pts3 - pts1).length();

	double uPow = pow(u, 2);
	double vPow = pow(v, 2);
	double wPow = pow(w, 2);
	double UPow = pow(U, 2);
	double VPow = pow(V, 2);
	double WPow = pow(W, 2);

	double a = 4 * (uPow * vPow * wPow) - uPow * pow((vPow + wPow - UPow), 2) - vPow * pow((wPow + uPow - VPow), 2) - wPow * pow((uPow + vPow - WPow), 2) + (vPow + wPow - UPow) * (wPow + uPow - VPow) * (uPow + vPow - WPow);
	double vol = sqrt(a);
	vol /= 12.0;
	return vol;
}

Vector3<double> Metrics::TetrahedronCentroid(Vector3<double> &pts0, Vector3<double> &pts1, Vector3<double> &pts2, Vector3<double> &pts3) {
	return (pts0 + pts1 + pts2 + pts3) / 4.0;
}
