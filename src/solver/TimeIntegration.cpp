//
// Created by amin on 3/7/21.
//

#include "solver/TimeIntegration.h"

using namespace E3D::Solver;
	double E3D::Solver::TimeStepMethod2(E3D::Solver::FlowField &_localFlowField,
	                const E3D::Mesh<E3D::Parser::MeshPartition> &_localMesh,
	                const E3D::Metrics &_localMetrics,
	                const int iElem) {
		double cfl = _localFlowField.getCFL();
		double sumSx = 0;
		double sumSy = 0;
		double sumSz = 0;

		int nfacefelement;


        int *ptr = _localMesh.GetElement2FaceID(iElem, nfacefelement);

		int iface;
		for (int i = 0; i < nfacefelement; i++) {
			iface = ptr[i];
			E3D::Vector3<double> faceNormals = _localMetrics.getFaceNormals()[iface];
			sumSx += std::abs(faceNormals.x);
			sumSy += std::abs(faceNormals.y);
			sumSz += std::abs(faceNormals.z);
		}


		double c = sqrt(_localFlowField.getgamma_ref() * _localFlowField.GetP()[iElem] / _localFlowField.Getrho()[iElem]);

		double lambdaCx = 0.5 * (std::abs(_localFlowField.GetU_Velocity()[iElem]) + c) * sumSx;
		double lambdaCy = 0.5 * (std::abs(_localFlowField.GetV_Velocity()[iElem]) + c) * sumSy;
		double lambdaCz = 0.5 * (std::abs(_localFlowField.GetW_Velocity()[iElem]) + c) * sumSz;

		double localTimeStep = cfl * _localMetrics.getCellVolumes()[iElem] / (lambdaCx + lambdaCy + lambdaCz);
		return localTimeStep;
	};



double E3D::Solver::TimeStepMethod1(E3D::Solver::FlowField &_localFlowField,
                                    const E3D::Mesh<E3D::Parser::MeshPartition> &_localMesh,
                                    const E3D::Metrics &_localMetrics,
                                    const int iElem) {



    int nfacefelement;
    int *ptr = _localMesh.GetElement2FaceID(iElem, nfacefelement);
    double lmbdaC=0;
    for (int i = 0; i < nfacefelement; i++) {

		int iface = ptr[1];
		int* f_ptr = _localMesh.GetFace2ElementID(iface);
		int elem0 = f_ptr[0];
        int elem1 = f_ptr[1];

		double rhoAvg = (_localFlowField.Getrho()[elem0] + _localFlowField.Getrho()[elem1])/2.0;
		double Uavg = (_localFlowField.GetU_Velocity()[elem0] + _localFlowField.GetU_Velocity()[elem1])/2.0;
		double Vavg = (_localFlowField.GetV_Velocity()[elem0] + _localFlowField.GetV_Velocity()[elem1])/2.0;
        double Wavg = (_localFlowField.GetW_Velocity()[elem0] + _localFlowField.GetW_Velocity()[elem1])/2.0;
		double pAvg = (_localFlowField.GetP()[elem0] + _localFlowField.GetP()[elem1])/2.0;

        double c = sqrt(_localFlowField.getgamma_ref() * pAvg / rhoAvg);

		Vector3<double> UnitFace = _localMetrics.getFaceNormalsUnit()[iface];

		double temp_sum = (std::abs(Uavg*UnitFace.x
		                           + Vavg*UnitFace.y
		                           + Wavg*UnitFace.z) + c ) * (_localMetrics.getFaceSurfaces()[iface]);
		lmbdaC += temp_sum;

	}

	double localTimeStep = _localMetrics.getCellVolumes()[iElem] * _localFlowField.getCFL() / lmbdaC;
	return localTimeStep;

}


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
