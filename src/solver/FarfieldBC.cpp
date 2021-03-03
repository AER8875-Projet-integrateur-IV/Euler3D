//
// Created by amin on 3/3/21.
//

#include "solver/FarfieldBC.h"


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
                                const int InteriorCellID) {

}