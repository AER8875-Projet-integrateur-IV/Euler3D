//
// Created by amin on 2/21/21.
//
#include "mesh/Metrics.hpp"
#include <cmath>
#include <numeric>


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
	reorientFaceVectors();


	//reorientFaceVectors();
	double endMetricsTimer = MPI_Wtime();


	double domainVolume = 0;

	double PartitionVolume = std::accumulate(_cellVolumes.begin(), _cellVolumes.end(), 0.0);
	MPI_Reduce(&PartitionVolume, &domainVolume, 1, MPI_DOUBLE_PRECISION, MPI_SUM, 0, MPI_COMM_WORLD);

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
			temp_centroid = (temp_LocalNodesCoords[0] + temp_LocalNodesCoords[1] + temp_LocalNodesCoords[2]) / 3;// Multiplication cheaper than divison

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
		                                             Vector3<double>(0.0, 0.0, 0.0)) /
		                             temp_LocalNodesCoords.size();


		for (int ifaceLocal = 0; ifaceLocal < numberOfConnectedFaces; ifaceLocal++) {
			// Prepare temporary vectors for computation
			Vector3<double> faceNormalVector = _faceNormals[p_localFaces[ifaceLocal]];
			Vector3<double> faceCenter = _faceCenters[p_localFaces[ifaceLocal]];
			Vector3<double> faceUnitVector = _faceUnitNormals[p_localFaces[ifaceLocal]];
			Vector3<double> faceToCellCenter = cellCenter - faceCenter;
			double surfaceLength = faceNormalVector.length();

			// Check if cell normal vector is pointing outward the element
			// Cos(theta) between normal face vector and CellCenter-to-facecenter is computed
			// if cos(theta) < 0 invert face Normal direction
			double CosthetaCellCenterFace = Vector3<double>::dot(faceToCellCenter, faceNormalVector) / (faceToCellCenter.length() * faceNormalVector.length());
			if (CosthetaCellCenterFace > 0) {
				faceNormalVector *= -1;
				faceUnitVector *= -1;
			}

			// Compute Centroid
			centroid_numerator += (faceCenter * Vector3<double>::dot(faceCenter, faceUnitVector) * surfaceLength) * 3;
			centroid_denominator += (Vector3<double>::dot(faceCenter, faceUnitVector) * surfaceLength) * 4;
		}
		temp_centroid = centroid_numerator / centroid_denominator;


		// Volume for hexahedron
		if (temp_localNodes.size() == 8) {
			Vector3<double> AB = temp_LocalNodesCoords[1] - temp_LocalNodesCoords[0];
			Vector3<double> AC = temp_LocalNodesCoords[2] - temp_LocalNodesCoords[0];
			Vector3<double> AD = temp_LocalNodesCoords[3] - temp_LocalNodesCoords[0];

			double temp_area = computeTriangleArea(AB, AC) + computeTriangleArea(AC, AD);
			double distanceBetweenFaces = (temp_LocalNodesCoords[0] - temp_LocalNodesCoords[4]).length();
			temp_volume = temp_area * distanceBetweenFaces;


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
			Vector3<double> AB = temp_LocalNodesCoords[1] - temp_LocalNodesCoords[0];
			Vector3<double> AC = temp_LocalNodesCoords[2] - temp_LocalNodesCoords[0];
			double temp_area = computeTriangleArea(AB, AC);
			double distanceBetweenFaces = (temp_LocalNodesCoords[0] - temp_LocalNodesCoords[3]).length();
			temp_volume = temp_area * distanceBetweenFaces;

			//Centroid
			double sumx = 0.0;
			double sumy = 0.0;
			double sumz = 0.0;

			for (int i = 0; i < 6; i++) {
				sumx += temp_LocalNodesCoords[i].x;
				sumy += temp_LocalNodesCoords[i].y;
				sumz += temp_LocalNodesCoords[i].z;
			}

			temp_centroid.x = sumx / 6.0;
			temp_centroid.y = sumy / 6.0;
			temp_centroid.z = sumz / 6.0;
		}

		// Pyramid
		else if (temp_localNodes.size() == 5) {
			//Volume
			Vector3<double> AB = temp_LocalNodesCoords[1] - temp_LocalNodesCoords[0];
			Vector3<double> AC = temp_LocalNodesCoords[2] - temp_LocalNodesCoords[0];
			Vector3<double> AD = temp_LocalNodesCoords[3] - temp_LocalNodesCoords[0];

			double temp_area = computeTriangleArea(AB, AC) + computeTriangleArea(AC, AD);//area of base
			//Plane equation is ax+by+cz+d=0
			double a = AB.y * AD.z - AB.z * AD.y;
			double b = -(AB.x * AD.z - AB.z * AD.x);
			double c = AB.x * AD.y - AB.y * AD.x;
			double d = -a * temp_LocalNodesCoords[0].x - b * temp_LocalNodesCoords[0].y - c * temp_LocalNodesCoords[0].z;

			double height = std::abs(a * temp_LocalNodesCoords[4].x + b * temp_LocalNodesCoords[4].y + c * temp_LocalNodesCoords[4].z + d) / (pow(pow(a, 2) + pow(b, 2) + pow(c, 2), 0.5));
			temp_volume = temp_area * height / 3.0;

			//Centroid
			double sumx = 0.0;
			double sumy = 0.0;
			double sumz = 0.0;

			for (int i = 0; i < 5; i++) {
				sumx += temp_LocalNodesCoords[i].x;
				sumy += temp_LocalNodesCoords[i].y;
				sumz += temp_LocalNodesCoords[i].z;
			}

			temp_centroid.x = sumx / 5.0;
			temp_centroid.y = sumy / 5.0;
			temp_centroid.z = sumz / 5.0;

		}

		//Volume and centroid for Tetrahedron
		else if (temp_LocalNodesCoords.size() == 4) {
			//Centroid
			Vector3<double> sumNodes = {0.0, 0.0, 0.0};
			for (int i = 0; i < 4; i++) {
				sumNodes += temp_LocalNodesCoords[i];
			}
			temp_centroid = sumNodes / 4.0;


			//Volume
			double U = (temp_LocalNodesCoords[0] - temp_LocalNodesCoords[1]).length();
			double V = (temp_LocalNodesCoords[1] - temp_LocalNodesCoords[2]).length();
			double W = (temp_LocalNodesCoords[2] - temp_LocalNodesCoords[0]).length();
			double u = (temp_LocalNodesCoords[2] - temp_LocalNodesCoords[3]).length();
			double v = (temp_LocalNodesCoords[0] - temp_LocalNodesCoords[3]).length();
			double w = (temp_LocalNodesCoords[3] - temp_LocalNodesCoords[1]).length();

			double uPow = pow(u, 2);
			double vPow = pow(v, 2);
			double wPow = pow(w, 2);
			double UPow = pow(U, 2);
			double VPow = pow(V, 2);
			double WPow = pow(W, 2);

			double a = 4 * (uPow * vPow * wPow) - uPow * pow((vPow + wPow - UPow), 2) - vPow * pow((wPow + uPow - VPow), 2) - wPow * pow((uPow + vPow - WPow), 2) + (vPow + wPow - UPow) * (wPow + uPow - VPow) * (uPow + vPow - WPow);
			double vol = sqrt(a);
			vol /= 12.0;
			temp_volume = vol;
		}

		_cellCentroids.push_back(temp_centroid);

		_cellVolumes.push_back(temp_volume);
	}
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

		double CosthetaCellCenterFace = Vector3<double>::dot(From0to1, _faceNormals[iface]) / (From0to1.length() * _faceNormals[iface].length());
		// IF connected to interior cell
		if (elem1 < _localMesh.GetMeshInteriorElemCount()) {

			E3D::Vector3<double> Coord2 = _cellCentroids[elem1];
			E3D::Vector3<double> From0to2 = Coord2 - Coord0;
			double CosthetaCellCenterFace2 = Vector3<double>::dot(From0to2, _faceNormals[iface]) / (From0to2.length() * _faceNormals[iface].length());
			if (CosthetaCellCenterFace > 0 && CosthetaCellCenterFace2 < 0) {
				_faceNormals[iface] *= -1;
				_faceUnitNormals[iface] *= -1;
			}
		}
		// If connected to ghost cell
		else {
			if (CosthetaCellCenterFace > 0) {
				_faceNormals[iface] *= -1;
				_faceUnitNormals[iface] *= -1;
			}
		}
	}
}