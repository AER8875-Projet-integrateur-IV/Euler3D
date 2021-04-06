//
// Created by amin on 3/3/21.
//

#include "solver/PhysicalBC.h"
#include "solver/ConvectiveFluxesSchemes.h"

using namespace E3D::Solver;


void BC::FarfieldSupersonicInflow(E3D::Solver::FlowField &flowfield, const int GhostcellID) {

	flowfield.setRho(GhostcellID, flowfield.getrho_inf());

	flowfield.setU(GhostcellID, flowfield.getu_inf());
	flowfield.setV(GhostcellID, flowfield.getv_inf());
	flowfield.setW(GhostcellID, flowfield.getw_inf());
	flowfield.setM(GhostcellID, flowfield.getM_inf());

	flowfield.setP(GhostcellID, flowfield.getp_inf());
	flowfield.setE(GhostcellID, flowfield.getE_inf());
	flowfield.setH(GhostcellID, flowfield.getH_inf());
}


void BC::FarfieldSupersonicOutflow(E3D::Solver::FlowField &flowfield,
                                   const int GhostcellID,
                                   const int InteriorCellID) {


	flowfield.setRho(GhostcellID, flowfield.Getrho()[InteriorCellID]);
	flowfield.setU(GhostcellID, flowfield.GetU_Velocity()[InteriorCellID]);
	flowfield.setV(GhostcellID, flowfield.GetV_Velocity()[InteriorCellID]);
	flowfield.setW(GhostcellID, flowfield.GetW_Velocity()[InteriorCellID]);
	flowfield.setM(GhostcellID, flowfield.GetMach()[InteriorCellID]);

	flowfield.setP(GhostcellID, flowfield.GetP()[InteriorCellID]);
	flowfield.setE(GhostcellID, flowfield.GetE()[InteriorCellID]);
	flowfield.setH(GhostcellID, flowfield.GetH()[InteriorCellID]);
}


void BC::FarfieldSubsonicInflow(E3D::Solver::FlowField &flowfield,
                                const E3D::Metrics &localMetrics,
                                const int GhostcellID,
                                const int InteriorCellID,
                                const int FaceID) {

	// Calculate primitive Variables
	double InteriorCellSpd = sqrt(flowfield.getgamma_ref() * (flowfield.GetP()[InteriorCellID] / flowfield.Getrho()[InteriorCellID]));

	double p = 0.5 * (flowfield.getp_inf() + flowfield.GetP()[InteriorCellID] - flowfield.Getrho()[InteriorCellID] * InteriorCellSpd * ((localMetrics.getFaceNormalsUnit()[FaceID].x * (flowfield.getu_inf() - flowfield.GetU_Velocity()[InteriorCellID])) + (localMetrics.getFaceNormalsUnit()[FaceID].y * (flowfield.getv_inf() - flowfield.GetV_Velocity()[InteriorCellID])) + (localMetrics.getFaceNormalsUnit()[FaceID].z * (flowfield.getw_inf() - flowfield.GetW_Velocity()[InteriorCellID]))));

	double rho = flowfield.getrho_inf() + (p - flowfield.getp_inf()) / std::pow(InteriorCellSpd, 2);

	double u = flowfield.getu_inf() - localMetrics.getFaceNormalsUnit()[FaceID].x * (flowfield.getp_inf() - p) / (flowfield.Getrho()[InteriorCellID] * InteriorCellSpd);

	double v = flowfield.getv_inf() - localMetrics.getFaceNormalsUnit()[FaceID].y * (flowfield.getp_inf() - p) / (flowfield.Getrho()[InteriorCellID] * InteriorCellSpd);

	double w = flowfield.getw_inf() - localMetrics.getFaceNormalsUnit()[FaceID].z * (flowfield.getp_inf() - p) / (flowfield.Getrho()[InteriorCellID] * InteriorCellSpd);

	double M = std::sqrt(std::pow(u, 2) + std::pow(v, 2) + std::pow(w, 2)) / InteriorCellSpd;

	double E = p / ((flowfield.getgamma_ref() - 1) * rho) + ((u * u + v * v + w * w) / 2);

	double H = E + (p / rho);

	// Update FlowField
	flowfield.setRho(GhostcellID, rho);

	flowfield.setU(GhostcellID, u);
	flowfield.setV(GhostcellID, v);
	flowfield.setW(GhostcellID, w);
	flowfield.setM(GhostcellID, M);

	flowfield.setP(GhostcellID, p);
	flowfield.setE(GhostcellID, E);
	flowfield.setH(GhostcellID, H);
}


void BC::FarfieldSubsonicOutflow(E3D::Solver::FlowField &flowfield,
                                 const E3D::Metrics &localMetrics,
                                 const int GhostcellID,
                                 const int InteriorCellID,
                                 const int FaceID) {

	// Calculate primitive Variables

	double InteriorCellSpd = sqrt(flowfield.getgamma_ref() * (flowfield.GetP()[InteriorCellID] / flowfield.Getrho()[InteriorCellID]));

	double p = flowfield.getp_inf();

	double rho = flowfield.Getrho()[InteriorCellID] + (p - flowfield.GetP()[InteriorCellID]) / std::pow(InteriorCellSpd, 2);

	double u = flowfield.GetU_Velocity()[InteriorCellID] + localMetrics.getFaceNormalsUnit()[FaceID].x * (flowfield.GetP()[InteriorCellID] - p) / (flowfield.Getrho()[InteriorCellID] * InteriorCellSpd);

	double v = flowfield.GetV_Velocity()[InteriorCellID] + localMetrics.getFaceNormalsUnit()[FaceID].y * (flowfield.GetP()[InteriorCellID] - p) / (flowfield.Getrho()[InteriorCellID] * InteriorCellSpd);

	double w = flowfield.GetW_Velocity()[InteriorCellID] + localMetrics.getFaceNormalsUnit()[FaceID].z * (flowfield.GetP()[InteriorCellID] - p) / (flowfield.Getrho()[InteriorCellID] * InteriorCellSpd);

	double M = std::sqrt(std::pow(u, 2) + std::pow(v, 2) + std::pow(w, 2)) / InteriorCellSpd;

	double E = p / ((flowfield.getgamma_ref() - 1) * rho) + ((u * u + v * v + w * w) / 2);

	double H = E + (p / rho);


	// Update FlowField
	flowfield.setRho(GhostcellID, rho);

	flowfield.setU(GhostcellID, u);
	flowfield.setV(GhostcellID, v);
	flowfield.setW(GhostcellID, w);
	flowfield.setM(GhostcellID, M);

	flowfield.setP(GhostcellID, p);
	flowfield.setE(GhostcellID, E);
	flowfield.setH(GhostcellID, H);
	//TODO MAYBE ADD LINEAR EXTRAPOLATION FROM STATES B AND D IF NEEDED
}


void BC::Wall(E3D::Solver::FlowField &flowfield,
              const E3D::Metrics &localMetrics,
              const int GhostcellID,
              const int InteriorCellID,
              const int FaceID) {

	double InteriorCellSpd = sqrt(flowfield.getgamma_ref() * (flowfield.GetP()[InteriorCellID] / flowfield.Getrho()[InteriorCellID]));

	double V = flowfield.GetU_Velocity()[InteriorCellID] * localMetrics.getFaceNormalsUnit()[FaceID].x + flowfield.GetV_Velocity()[InteriorCellID] * localMetrics.getFaceNormalsUnit()[FaceID].y + flowfield.GetW_Velocity()[InteriorCellID] * localMetrics.getFaceNormalsUnit()[FaceID].z;

	double u = flowfield.GetU_Velocity()[InteriorCellID] - 2 * V * localMetrics.getFaceNormalsUnit()[FaceID].x;

	double v = flowfield.GetV_Velocity()[InteriorCellID] - 2 * V * localMetrics.getFaceNormalsUnit()[FaceID].y;

	double w = flowfield.GetW_Velocity()[InteriorCellID] - 2 * V * localMetrics.getFaceNormalsUnit()[FaceID].z;

	double p = flowfield.GetP()[InteriorCellID];

	double rho = flowfield.Getrho()[InteriorCellID];

	double M = std::sqrt(std::pow(u, 2) + std::pow(v, 2) + std::pow(w, 2)) / InteriorCellSpd;

	double E = p / ((flowfield.getgamma_ref() - 1) * rho) + ((u * u + v * v + w * w) / 2.0);

	double H = E + (p / rho);


	// Update FlowField
	flowfield.setRho(GhostcellID, rho);

	flowfield.setU(GhostcellID, u);
	flowfield.setV(GhostcellID, v);
	flowfield.setW(GhostcellID, w);
	flowfield.setM(GhostcellID, M);

	flowfield.setP(GhostcellID, p);
	flowfield.setE(GhostcellID, E);
	flowfield.setH(GhostcellID, H);
}


void BC::Symmetry(E3D::Solver::FlowField &flowfield,
                  const E3D::Metrics &localMetrics,
                  const int GhostcellID,
                  const int InteriorCellID,
                  const int FaceID) {

	//	double u = flowfield.GetU_Velocity()[InteriorCellID];
	//
	//	double v = flowfield.GetV_Velocity()[InteriorCellID];
	//
	//	double w = flowfield.GetW_Velocity()[InteriorCellID];
	//
	//	double p = flowfield.GetP()[InteriorCellID];
	//
	//	double rho = flowfield.Getrho()[InteriorCellID];
	//
	//	double E = flowfield.GetE()[InteriorCellID];
	//
	//	double H = flowfield.GetH()[InteriorCellID];
	//
	//	double M = flowfield.GetMach()[InteriorCellID];
	//
	//	// Update FlowField
	//	flowfield.setRho(GhostcellID, rho);
	//
	//	flowfield.setU(GhostcellID, u);
	//	flowfield.setV(GhostcellID, v);
	//	flowfield.setW(GhostcellID, w);
	//	flowfield.setM(GhostcellID, M);
	//
	//	flowfield.setP(GhostcellID, p);
	//	flowfield.setE(GhostcellID, E);
	//	flowfield.setH(GhostcellID, H);

	double InteriorCellSpd = sqrt(flowfield.getgamma_ref() * (flowfield.GetP()[InteriorCellID] / flowfield.Getrho()[InteriorCellID]));

	double V = flowfield.GetU_Velocity()[InteriorCellID] * localMetrics.getFaceNormalsUnit()[FaceID].x + flowfield.GetV_Velocity()[InteriorCellID] * localMetrics.getFaceNormalsUnit()[FaceID].y + flowfield.GetW_Velocity()[InteriorCellID] * localMetrics.getFaceNormalsUnit()[FaceID].z;

	double u = flowfield.GetU_Velocity()[InteriorCellID] - 2 * V * localMetrics.getFaceNormalsUnit()[FaceID].x;

	double v = flowfield.GetV_Velocity()[InteriorCellID] - 2 * V * localMetrics.getFaceNormalsUnit()[FaceID].y;

	double w = flowfield.GetW_Velocity()[InteriorCellID] - 2 * V * localMetrics.getFaceNormalsUnit()[FaceID].z;

	double p = flowfield.GetP()[InteriorCellID];

	double rho = flowfield.Getrho()[InteriorCellID];

	double M = std::sqrt(std::pow(u, 2) + std::pow(v, 2) + std::pow(w, 2)) / InteriorCellSpd;

	double E = p / ((flowfield.getgamma_ref() - 1) * rho) + ((u * u + v * v + w * w) / 2.0);

	double H = E + (p / rho);


	// Update FlowField
	flowfield.setRho(GhostcellID, rho);

	flowfield.setU(GhostcellID, u);
	flowfield.setV(GhostcellID, v);
	flowfield.setW(GhostcellID, w);
	flowfield.setM(GhostcellID, M);

	flowfield.setP(GhostcellID, p);
	flowfield.setE(GhostcellID, E);
	flowfield.setH(GhostcellID, H);
}