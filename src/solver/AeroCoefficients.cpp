#include "solver/AeroCoefficients.hpp"

using namespace E3D::Solver;

AeroCoefficients::AeroCoefficients(double pInf,
                                   double rhoInf,
                                   const E3D::Vector3<double> &uInf,
                                   const E3D::Mesh<E3D::Parser::MeshPartition> &mesh,
                                   const E3D::Metrics &metrics,
                                   const E3D::Vector3<double> &refPoint,
                                   double Sref,
                                   double Cref) : _Sref(Sref), _Cref(Cref) {

	_ReferencePoint = refPoint;
	_BorderCells = mesh.GetWallAdjacentGhostCellIDs();
	const std::vector<int> &BorderFaces = mesh.GetWallAdjacentFaceIDs();
	const std::vector<E3D::Vector3<double>> &normals = metrics.getFaceNormals();
	const std::vector<E3D::Vector3<double>> &centroids = metrics.getFaceCenters();

	_nElem = _BorderCells.size();
	_FaceVectors.reserve(_nElem);
	_FaceCentroids.reserve(_nElem);
	double uInfSquared = E3D::Vector3<double>::dot(uInf, uInf);
	_pInf = pInf;
	_cpFactor = 1 / (0.5 * rhoInf * uInfSquared);
	_ForceCoeff = E3D::Vector3<double>(0., 0., 0.);
	_cp.resize(_nElem);
	_SumFaceVector *= 0;
	for (int iElem = 0; iElem < _nElem; iElem++) {
		E3D::Vector3<double> vec = normals[BorderFaces[iElem]];
		_SumFaceVector += vec;
		_FaceVectors.push_back(vec);
		_FaceCentroids.push_back(centroids[BorderFaces[iElem]]);
	}
}


void AeroCoefficients::Update(const std::vector<double> &pressure) {
	_ForceCoeff *= 0;
	_CentrePressure *= 0;
	for (int i = 0; i < _nElem; i++) {
		int cellID = _BorderCells[i];
		_cp[i] = _cpFactor * (pressure[cellID] - _pInf);
		E3D::Vector3<double> coeff = _FaceVectors[i] * _cp[i] * _Cref / _Sref;
		_ForceCoeff += coeff;
		_CentrePressure += coeff * _FaceCentroids[i];
	}
	_CentrePressure /= _ForceCoeff;
	E3D::Vector3 lever = (_ReferencePoint - _CentrePressure);
	_MomentCoeff = E3D::Vector3<double>::crossProduct(_ForceCoeff, lever);
	_MomentCoeff /= _Cref;
}