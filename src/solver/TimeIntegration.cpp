//
// Created by amin on 3/7/21.
//

#include "solver/TimeIntegration.h"

using namespace E3D::Solver;
	double E3D::Solver::TimeStep(E3D::Solver::FlowField &_localFlowField,
	                const E3D::Mesh<E3D::Parser::MeshPartition> &_localMesh,
	                const E3D::Metrics &_localMetrics,
	                const int iElem) {
		double cfl = _localFlowField.getCFL();
		double sumSx = 0;
		double sumSy = 0;
		double sumSz = 0;

		int nfacefelement;
		int *ptr;

		ptr = _localMesh.GetElement2FaceID(iElem, nfacefelement);
		std::vector<int> element2face(ptr, ptr + nfacefelement);

		int iface;
		for (size_t i = 0; i < nfacefelement; i++) {
			iface = element2face[i];
			E3D::Vector3<double> faceNormals = _localMetrics.getFaceNormals()[iface];
			sumSx += abs(faceNormals.x * _localMetrics.getFaceSurfaces()[iface]);
			sumSy += abs(faceNormals.y * _localMetrics.getFaceSurfaces()[iface]);
			sumSz += abs(faceNormals.z * _localMetrics.getFaceSurfaces()[iface]);
		}

		double c = sqrt(_localFlowField.getgamma_ref() * _localFlowField.GetP()[iElem] / _localFlowField.Getrho()[iElem]);
		double lambdaCx = 0.5 * (abs(_localFlowField.GetU_Velocity()[iElem]) + c) * sumSx;
		double lambdaCy = 0.5 * (abs(_localFlowField.GetV_Velocity()[iElem]) + c) * sumSy;
		double lambdaCz = 0.5 * (abs(_localFlowField.GetW_Velocity()[iElem]) + c) * sumSz;

		double localTimeStep = cfl * _localMetrics.getCellVolumes()[iElem] / (lambdaCx + lambdaCy + lambdaCz);

		return localTimeStep;
	};

E3D::Solver::ConservativeVar E3D::Solver::ExplicitEuler(E3D::Solver::ResidualVar residual,
                                           double dt,
                                           const E3D::Metrics &_localMetrics,
                                           const int iElem) {

	double coeff = -dt / _localMetrics.getCellVolumes()[iElem];
	E3D::Solver::ResidualVar temp_deltaW = residual * coeff;
	E3D::Solver::ConservativeVar deltaW { temp_deltaW.m_rhoV_residual,
		                                  temp_deltaW.m_rho_uV_residual,
		                                  temp_deltaW.m_rho_vV_residual,
		                                  temp_deltaW.m_rho_wV_residual,
		                                  temp_deltaW.m_rho_HV_residual };
	return deltaW;
}
