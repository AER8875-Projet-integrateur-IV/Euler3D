#include "solver/AeroCoefficients.hpp"

using namespace E3D::Solver;

AeroCoefficients::AeroCoefficients(double pInf,
                                   double rhoInf,
                                   const E3D::Vector3<double> &uInf,
                                   const std::vector<E3D::Vector3<double>> &faceVectors) {
	double uInfSquared = E3D::Vector3<double>::dot(uInf, uInf);
	_cpFactor = 1 / (0.5 * rhoInf * uInfSquared);
	_nElem = faceVectors.size();
	_FaceVectors = faceVectors;
	_ForceCoeff = E3D::Vector3<double>(0., 0., 0.);
	_cp.resize(_nElem);
	_pInf = pInf;
}


AeroCoefficients::AeroCoefficients(double pInf,
                                   double rhoInf,
                                   const E3D::Vector3<double> &uInf,
                                   const E3D::Mesh<E3D::Parser::MeshPartition> &mesh,
                                   const E3D::Metrics &metrics) {

	// Make sure all faceVectors are facing towards the outside of the domain
	// const std::vector<int> &GhostCells = mesh.GetWallGhostCellsIDs();
	_BorderCells = mesh.GetWallAdjacentGhostCellIDs();
	const std::vector<int> &BorderFaces = mesh.GetWallAdjacentFaceIDs();
	// const std::vector<E3D::Vector3<double>> &centroids = metrics.getCellCentroids();
	const std::vector<E3D::Vector3<double>> &normals = metrics.getFaceNormals();

	// std::cout << uInf.x << " | " << uInf.y << " | " << uInf.z << std::endl;

	_nElem = _BorderCells.size();
	_FaceVectors.reserve(_nElem);
	double uInfSquared = E3D::Vector3<double>::dot(uInf, uInf);
	_pInf = pInf;
	_cpFactor = 1 / (0.5 * rhoInf * uInfSquared);
	_ForceCoeff = E3D::Vector3<double>(0., 0., 0.);
	_cp.resize(_nElem);
	// std::cout << "_pInf: " << _pInf << "\n";
	// std::cout << "_cpfactor: " << _cpFactor << "\n";
	// std::cout << "number of wall elements: " << _nElem << "\n";
	// E3D::Vector3<double> TEST(0, 0, 0);

	for (int iElem = 0; iElem < _nElem; iElem++) {
		// E3D::Vector3<double> outerFacingVector;
		E3D::Vector3<double> faceVector;

		faceVector = normals[BorderFaces[iElem]];

		// const E3D::Vector3<double> outerFacingVector = centroids[GhostCells[iElem]] - centroids[BorderCells[iElem]];

		// if (E3D::Vector3<double>::dot(outerFacingVector, faceVector) > 0) {
		// 	// facing out of the domain
		_FaceVectors.push_back(faceVector);
		// } else {
		// 	//facing in the domain
		// 	_FaceVectors.push_back(faceVector * -1);
		// }
		// TEST += _FaceVectors[iElem];
	}
	// std::cout << TEST.x << " | " << TEST.y << " | " << TEST.z << std::endl;
}

void AeroCoefficientspreProcessing(double pInf,
                                   double rhoInf,
                                   const E3D::Vector3<double> &uInf) {
}

E3D::Vector3<double> &AeroCoefficients::SolveCoefficients(const std::vector<double> &pressure) {
	_ForceCoeff *= 0;
	for (int i = 0; i < _nElem; i++) {
		int cellID = _BorderCells[i];
		_cp[i] = _cpFactor * (pressure[cellID] - _pInf);
		_ForceCoeff += _FaceVectors[i] * _cp[i];
	}

	return _ForceCoeff;
}