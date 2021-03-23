//
// Created by amin on 3/7/21.
//


#include "solver/ConvectiveFluxesSchemes.h"

using namespace E3D::Solver;

E3D::Solver::ResidualVar E3D::Solver::Roe(E3D::Solver::FlowField &_localFlowField,
                                          const E3D::Mesh<E3D::Parser::MeshPartition> &_localMesh,
                                          const E3D::Metrics &_localMetrics,
                                          const int iface) {

	int *ptr = _localMesh.GetFace2ElementID(iface);
	int leftElementId = ptr[0];
	int rightElementId = ptr[1];


	//get variables from FlowField
	double gamma = _localFlowField.getgamma_ref();

	double leftRho = _localFlowField.Getrho()[leftElementId];//weird de donner tout le vector, pourquoi pas juste la valeur?
	double rightRho = _localFlowField.Getrho()[rightElementId];

	double sqrtLeftRho = sqrt(leftRho);
	double sqrtRightRho = sqrt(rightRho);

	//calculate variables for flowfield
	double rhoTilde = sqrt(leftRho * rightRho);

	double leftU = _localFlowField.GetU_Velocity()[leftElementId];
	double rightU = _localFlowField.GetU_Velocity()[rightElementId];

	double leftV = _localFlowField.GetV_Velocity()[leftElementId];
	double rightV = _localFlowField.GetV_Velocity()[rightElementId];

	double leftW = _localFlowField.GetW_Velocity()[leftElementId];
	double rightW = _localFlowField.GetW_Velocity()[rightElementId];

	double leftH = _localFlowField.GetH()[leftElementId];
	double rightH = _localFlowField.GetH()[rightElementId];

	double leftP = _localFlowField.GetP()[leftElementId];
	double rightP = _localFlowField.GetP()[rightElementId];


	double uTilde = (leftU * sqrtLeftRho + rightU * sqrtRightRho) / (sqrtLeftRho + sqrtRightRho);
	double vTilde = (leftV * sqrtLeftRho + rightV * sqrtRightRho) / (sqrtLeftRho + sqrtRightRho);
	double wTilde = (leftW * sqrtLeftRho + rightW * sqrtRightRho) / (sqrtLeftRho + sqrtRightRho);
	double hTilde = (leftH * sqrtLeftRho + rightH * sqrtRightRho) / (sqrtLeftRho + sqrtRightRho);


	double qTildeSquare = std::pow(uTilde, 2.0) + std::pow(vTilde, 2.0) + std::pow(wTilde, 2.0);
	double cTilde = sqrt((gamma - 1.0) * (hTilde - qTildeSquare / 2));

	//get variables from metrics
	E3D::Vector3<double> faceNormals = _localMetrics.getFaceNormalsUnit()[iface];

	double VContravariantTilde = uTilde * faceNormals.x + vTilde * faceNormals.y + wTilde * faceNormals.z;
	double leftVContravariant = leftU * faceNormals.x + leftV * faceNormals.y + leftW * faceNormals.z;
	double rightVContravariant = rightU * faceNormals.x + rightV * faceNormals.y + rightW * faceNormals.z;

	//calculate deltas
	double deltaRho = rightRho - leftRho;
	double deltaU = rightU - leftU;
	double deltaV = rightV - leftV;
	double deltaW = rightW - leftW;
	double deltaP = rightP - leftP;
	double deltaVContravariant = rightVContravariant - leftVContravariant;

	//	//calculate average variables
	//	double rhoAvg = 0.5 * (rightRho + leftRho);
	//	double uAvg = 0.5 * (rightU + leftU);
	//	double vAvg = 0.5 * (rightV + leftV);
	//	double wAvg = 0.5 * (rightW + leftW);
	//	double pAvg = 0.5 * (rightP + leftP);
	//	double hAvg = 0.5 * (rightH + leftH);
	//	double VContravariantAvg = uAvg * faceNormals.x + vAvg * faceNormals.y + wAvg * faceNormals.z;
	//	std::vector<double> fluxAvg = {rhoAvg * VContravariantAvg,
	//	                               rhoAvg * VContravariantAvg * uAvg + pAvg * faceNormals.x,
	//	                               rhoAvg * VContravariantAvg * vAvg + pAvg * faceNormals.y,
	//	                               rhoAvg * VContravariantAvg * wAvg + pAvg * faceNormals.z,
	//	                               rhoAvg * VContravariantAvg * hAvg};

	//calculate Flux
	std::array<double, 5> flux = {0, 0, 0, 0, 0};

	////	//Par Amin
	std::array<double, 5> fluxAvg{0.0, 0.0, 0.0, 0.0, 0.0};

	std::array<double, 5> Fcr = {rightRho * rightVContravariant,
	                             rightRho * rightVContravariant * rightU + rightP * faceNormals.x,
	                             rightRho * rightVContravariant * rightV + rightP * faceNormals.y,
	                             rightRho * rightVContravariant * rightW + rightP * faceNormals.z,
	                             rightRho * rightVContravariant * rightH};
	std::array<double, 5> Fcl = {leftRho * leftVContravariant,
	                             leftRho * leftVContravariant * leftU + leftP * faceNormals.x,
	                             leftRho * leftVContravariant * leftV + leftP * faceNormals.y,
	                             leftRho * leftVContravariant * leftW + leftP * faceNormals.z,
	                             leftRho * leftVContravariant * leftH};
	for (size_t i = 0; i < 5; i++) {
		fluxAvg[i] = (Fcr[i] + Fcl[i]) * 0.5;
	}


	double localC = sqrt(gamma * leftP / leftRho);
	double HartensCorrector = (1.0 / 15.0) * localC;
	//hartens correction
	double HartensCorrectionF1;
	if (std::abs(VContravariantTilde - cTilde) > HartensCorrector) {
		HartensCorrectionF1 = std::abs(VContravariantTilde - cTilde);
	} else {
		HartensCorrectionF1 = (std::pow(VContravariantTilde - cTilde, 2) + std::pow(HartensCorrector, 2)) / (2.0 * HartensCorrector);
	}

	double HartensCorrectionF5;
	if (std::abs(VContravariantTilde + cTilde) > HartensCorrector) {
		HartensCorrectionF5 = std::abs(VContravariantTilde + cTilde);
	} else {
		HartensCorrectionF5 = (std::pow(VContravariantTilde + cTilde, 2) + std::pow(HartensCorrector, 2)) / (2.0 * HartensCorrector);
	}

	//calculate F1, F234, F5 matrices
	std::array<double, 5> deltaF1 = {1.0,
	                                 uTilde - cTilde * faceNormals.x,
	                                 vTilde - cTilde * faceNormals.y,
	                                 wTilde - cTilde * faceNormals.z,
	                                 hTilde - cTilde * VContravariantTilde};
	for (size_t i = 0; i < 5; i++) {
		deltaF1[i] = deltaF1[i] * HartensCorrectionF1 * (deltaP - rhoTilde * cTilde * deltaVContravariant) / (2.0 * std::pow(cTilde, 2.0));
	}

	std::array<double, 5> deltaF234 = {1.0,
	                                   uTilde,
	                                   vTilde,
	                                   wTilde,
	                                   qTildeSquare * 0.5};
	for (size_t i = 0; i < 5; i++) {
		deltaF234[i] = deltaF234[i] * (deltaRho - deltaP / std::pow(cTilde, 2));
	}
	std::array<double, 5> deltaF234_2term = {0.0,
	                                         rhoTilde * (deltaU - deltaVContravariant * faceNormals.x),
	                                         rhoTilde * (deltaV - deltaVContravariant * faceNormals.y),
	                                         rhoTilde * (deltaW - deltaVContravariant * faceNormals.z),
	                                         rhoTilde * (uTilde * deltaU + vTilde * deltaV + wTilde * deltaW - VContravariantTilde * deltaVContravariant)};
	for (size_t i = 0; i < 5; i++) {
		deltaF234[i] = (deltaF234[i] + deltaF234_2term[i]) * std::abs(VContravariantTilde);
	}

	std::array<double, 5> deltaF5 = {1.0,
	                                 uTilde + cTilde * faceNormals.x,
	                                 vTilde + cTilde * faceNormals.y,
	                                 wTilde + cTilde * faceNormals.z,
	                                 hTilde + cTilde * VContravariantTilde};
	for (size_t i = 0; i < 5; i++) {
		deltaF5[i] = deltaF5[i] * HartensCorrectionF5 * (deltaP + rhoTilde * cTilde * deltaVContravariant) / (2.0 * std::pow(cTilde, 2.0));
	}

	for (size_t i = 0; i < 5; i++) {
		flux[i] = fluxAvg[i] - 0.5 * (deltaF1[i] + deltaF234[i] + deltaF5[i]);
	}

	return E3D::Solver::ResidualVar(flux[0], flux[1], flux[2], flux[3], flux[4]);
}

E3D::Solver::ResidualVar E3D::Solver::Fc(E3D::Solver::FlowField &localFlowField,
                                         const E3D::Metrics &localMetrics,
                                         const int GhostID,
                                         const int faceID,
                                         double V) {

	double rho = localFlowField.Getrho()[GhostID];
	double u = localFlowField.GetU_Velocity()[GhostID];
	double v = localFlowField.GetV_Velocity()[GhostID];
	double w = localFlowField.GetW_Velocity()[GhostID];
	double H = localFlowField.GetH()[GhostID];
	double p = localFlowField.GetP()[GhostID];
	Vector3<double> UnitFaceNormals = localMetrics.getFaceNormalsUnit()[faceID];
	double Fc1 = rho * V;
	double Fc2 = rho * u * V + p * UnitFaceNormals.x;
	double Fc3 = rho * v * V + p * UnitFaceNormals.y;
	double Fc4 = rho * w * V + p * UnitFaceNormals.z;
	double Fc5 = rho * H * V;

	return ResidualVar(Fc1, Fc2, Fc3, Fc4, Fc5);
}