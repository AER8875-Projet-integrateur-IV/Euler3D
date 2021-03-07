//
// Created by amin on 3/1/21.
//

#include "solver/EulerSolver.hpp"
#include "utils/SolverPrint.hpp"
#include <math.h>
#include "utils/Vector3.h"


using namespace E3D;

Solver::EulerSolver::EulerSolver(FlowField &localFlowField,
                                 const E3D::Parallel::MPIHandler &e3d_mpi,
                                 const E3D::Mesh<E3D::Parser::MeshPartition> &localMesh,
                                 const E3D::Parser::SimConfig &config,
                                 const E3D::Metrics& localMetrics)

    : _localFlowField(localFlowField), _e3d_mpi(e3d_mpi), _localMesh(localMesh), _config(config), _localMetrics(localMetrics) {

	if (e3d_mpi.getRankID() == 0) {
		std::cout << "\n\n"
		          << std::string(24, '#') << "  Starting Solving Process !  " << std::string(24, '#') << "\n\n";
	}


}

void Solver::EulerSolver::Run() {

	if (_e3d_mpi.getRankID() == 0) {
        E3D::Solver::printHeader();
	}

	while (_maximumLocalRms > _config.getMinResidual() && _nbInteration < _config.getMaxNumberIterations()) {


        double iterationBeginimer = MPI_Wtime();

		// loop Through Ghost cells (Boundary Cells)
        updateBC();

        _nbInteration += 1;


		if (_e3d_mpi.getRankID() == 0) {
			double iterationEndTimer = MPI_Wtime();
			double iterationwallTime = iterationEndTimer - iterationBeginimer;
			E3D::Solver::PrintSolverIteration(_CL, _CD, _maximumLocalRms, iterationwallTime, _nbInteration);
		}
		MPI_Barrier(MPI_COMM_WORLD);

	}

	// If loop exited because of reaching maximum number of iteration, print this message
	if (_config.getMaxNumberIterations() >= _nbInteration) {
		if (_e3d_mpi.getRankID() == 0) {
			std::cout << "Solution not converged :  exceeded maximum number of iterations !";
		}
	}
}




// Update Farfield, Wall, Symmetry and MPI ghost cell primitive values
void Solver::EulerSolver::updateBC() {

	// Update Farfield
    for (size_t GhostID=0; GhostID < _FarfieldGhostCellIDs.size(); GhostID++) {

		int GhostIdx = _FarfieldGhostCellIDs[GhostID];
		int FaceGhostIdx = _localMesh.GetFarfieldAdjacentFaceIDs()[GhostID];
		int InteriorGhostIdx = _localMesh.GetFarfieldAdjacentToGhostCellsIDs()[GhostID];

		double V = _localFlowField.GetU_Velocity()[GhostIdx]*_localMetrics.getFaceNormalsUnit()[FaceGhostIdx].x
		        + _localFlowField.GetV_Velocity()[GhostIdx]*_localMetrics.getFaceNormalsUnit()[FaceGhostIdx].y
		        + _localFlowField.GetW_Velocity()[GhostIdx]*_localMetrics.getFaceNormalsUnit()[FaceGhostIdx].z;

		double M = _localFlowField.GetMach()[GhostIdx];
		printf("V = %.3f, M=%.3f\n",V,M);

		if ( M > 1.0 &&  V > 0.0){
			Solver::BC::FarfieldSupersonicOutflow(_localFlowField,GhostIdx,InteriorGhostIdx);
		}
		else if( M < 1.0 && V > 0.0){
            Solver::BC::FarfieldSubsonicOutflow(_localFlowField,_localMetrics,GhostIdx,InteriorGhostIdx, FaceGhostIdx);
		}
		else if(M > 1.0 && V < 0.0){
			Solver::BC::FarfieldSupersonicInflow(_localFlowField,GhostIdx);
		}
		else if(M < 1.0 && V < 0.0) {
			Solver::BC::FarfieldSubsonicInflow(_localFlowField, _localMetrics, GhostIdx, InteriorGhostIdx, FaceGhostIdx);
		}

    }

	// Update Wall
    for (size_t GhostID=0; GhostID < _WallGhostCellIDs.size(); GhostID++) {
        int GhostIdx = _WallGhostCellIDs[GhostID];
        int FaceGhostIdx = _localMesh.GetWallAdjacentFaceIDs()[GhostID];
        int InteriorGhostIdx = _localMesh.GetWallAdjacentGhostCellIDs()[GhostID];

        Solver::BC::Wall(_localFlowField,_localMetrics,GhostIdx,InteriorGhostIdx,FaceGhostIdx);
    }

	// Update Symmetry
    for (size_t GhostID=0; GhostID < _SymmetryGhostCellIDs.size(); GhostID++) {

        int GhostIdx = _WallGhostCellIDs[GhostID];
        int InteriorGhostIdx = _localMesh.GetWallAdjacentGhostCellIDs()[GhostID];

        Solver::BC::Symmetry(_localFlowField,GhostIdx,InteriorGhostIdx);
    }

	// Update MPI
    _e3d_mpi.updateFlowField(_localFlowField);
}

void Solver::EulerSolver::computeResidual(int iface){

  int size = 2;
  int *ptr;

  ptr = _localMesh.GetFace2ElementID(iface);
  std::vector<int> elementsIds(ptr, ptr + size); //left is at 0
  int leftElementId = elementsIds[0];
  int rightElementId = elementsIds[1];

  //get variables from FlowField
  double gamma = _config.getGamma();

  double leftRho = _localFlowField.Getrho()[leftElementId];//weird de donner tout le vector, pourquoi pas juste la valeur?
  double rightRho = _localFlowField.Getrho()[rightElementId];

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


  //get variables from metrics
  E3D::Vector3<double> faceNormals = _localMetrics.getFaceNormals()[iface];

  //calculate variables for flowfield
  double rhoTilde = sqrt(leftRho * rightRho);
  double uTilde = (leftU*sqrt(leftRho) + rightU * sqrt(rightRho))/(sqrt(leftRho)+sqrt(rightRho));
  double vTilde = (leftV*sqrt(leftRho) + rightV * sqrt(rightRho))/(sqrt(leftRho)+sqrt(rightRho));
  double wTilde = (leftW*sqrt(leftRho) + rightW * sqrt(rightRho))/(sqrt(leftRho)+sqrt(rightRho));
  double hTilde = (leftH*sqrt(leftRho) + rightH * sqrt(rightRho))/(sqrt(leftRho)+sqrt(rightRho));

  double qTildeSquare = pow(uTilde,2.0) + pow(vTilde,2.0) + pow(wTilde,2.0);
  double cTilde = sqrt((gamma-1.0)*(hTilde-qTildeSquare/2));
  double VContravariantTilde = uTilde*faceNormals.x + vTilde*faceNormals.y + wTilde*faceNormals.z;
  double leftVContravariant = leftU*faceNormals.x + leftV*faceNormals.y + leftW*faceNormals.z;
  double rightVContravariant = rightU*faceNormals.x + rightV*faceNormals.y + rightW*faceNormals.z;

  //calculate deltas
  double deltaRho = rightRho - leftRho;
  double deltaU = rightU - leftU;
  double deltaV = rightV - leftV;
  double deltaW = rightW - leftW;
  double deltaP = rightP - leftP;
  double deltaVContravariant = rightVContravariant - leftVContravariant;

  //calculate average variables
  double rhoAvg = 0.5*(rightRho + leftRho);
  double uAvg = 0.5*(rightU + leftU);
  double vAvg = 0.5*(rightV + leftV);
  double wAvg = 0.5*(rightW + leftW);
  double pAvg = 0.5*(rightP + leftP);
  double hAvg = 0.5*(rightH + leftH);
  double VContravariantAvg = uAvg*faceNormals.x + vAvg*faceNormals.y + wAvg*faceNormals.z;
  std::vector<double> fluxAvg = {rhoAvg*VContravariantAvg,
                                 rhoAvg*VContravariantAvg*uAvg + pAvg*faceNormals.x,
                                 rhoAvg*VContravariantAvg*vAvg + pAvg*faceNormals.y,
                                 rhoAvg*VContravariantAvg*wAvg + pAvg*faceNormals.z,
                                 rhoAvg*VContravariantAvg*hAvg};

  //calculate Flux
  std::vector<double> flux = {0,0,0,0,0};
  if (iface > _localMesh.GetnFaceInt()) {
    for (size_t i = 0; i < 5; i++) {
      flux[i] = fluxAvg[i];
    }
  } else {
    double localC = sqrt(gamma*leftP/leftRho);
    double HartensCorrector = 1.0/15.0*localC;
    //hartens correction
    double HartensCorrectionF1;
    if (abs(VContravariantTilde-cTilde) > HartensCorrector) {
      HartensCorrectionF1 = abs(VContravariantTilde-cTilde);
    } else {
      HartensCorrectionF1 = (pow(VContravariantTilde-cTilde,2)+pow(HartensCorrector,2))/(2.0*HartensCorrector);
    }

    double HartensCorrectionF5;
    if (abs(VContravariantTilde+cTilde) > HartensCorrector) {
      HartensCorrectionF5 = abs(VContravariantTilde+cTilde);
    } else {
      HartensCorrectionF5 = (pow(VContravariantTilde+cTilde,2)+pow(HartensCorrector,2))/(2.0*HartensCorrector);
    }

    //calculate F1, F234, F5 matrices
    std::vector<double> deltaF1 = {1,
                              uTilde-cTilde*faceNormals.x,
                              vTilde-cTilde*faceNormals.y,
                              wTilde-cTilde*faceNormals.z,
                              hTilde - cTilde*VContravariantTilde};
    for (size_t i = 0; i < 5; i++) {
      deltaF1[i] = deltaF1[i]*HartensCorrectionF1*(deltaP - rhoTilde*cTilde*deltaVContravariant)/(2.0*pow(cTilde,2.0));
    }

    std::vector<double> deltaF234 = {1,
                                uTilde,
                                vTilde,
                                wTilde,
                                qTildeSquare/2.0};
    for (size_t i = 0; i < 5; i++) {
      deltaF234[i] = deltaF234[i]*(deltaRho - deltaP/pow(cTilde,2));
    }
    std::vector<double> deltaF234_2term = {0,
                                      rhoTilde*(deltaU - deltaVContravariant*faceNormals.x),
                                      rhoTilde*(deltaV - deltaVContravariant*faceNormals.y),
                                      rhoTilde*(deltaW - deltaVContravariant*faceNormals.z),
                                      rhoTilde*(uTilde*deltaU + vTilde*deltaV + wTilde*deltaW- VContravariantTilde*deltaVContravariant)};
    for (size_t i = 0; i < 5; i++) {
      deltaF234[i] = (deltaF234[i]+deltaF234_2term[i])*abs(VContravariantTilde);
    }

    std::vector<double> deltaF5 = {1,
                              uTilde+cTilde*faceNormals.x,
                              vTilde+cTilde*faceNormals.y,
                              wTilde+cTilde*faceNormals.z,
                              hTilde+cTilde*VContravariantTilde};
    for (size_t i = 0; i < 5; i++) {
      deltaF5[i] = deltaF5[i]*HartensCorrectionF5*(deltaP + rhoTilde*cTilde*deltaVContravariant)/(2.0*pow(cTilde,2.0));
    }

    for (size_t i = 0; i < 5; i++) {
      flux[i] = fluxAvg[i] - 0.5*(deltaF1[i] + deltaF234[i] + deltaF5[i]);
    }
  }










}
