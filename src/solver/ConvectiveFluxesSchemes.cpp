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

    Vector3<double> faceCenter = _localMetrics.getFaceCenters()[iface];
    int leftElementId = ptr[0];
    int rightElementId = ptr[1];

    bool borderElem;
    rightElementId < _localMesh.GetnElemTot() ? borderElem = false : borderElem = true;
    auto PrimitiveVarGrads = GreenGaussGradient(_localFlowField, _localMesh, _localMetrics, iface, borderElem);
    //auto limiters = VenkatakrishnanLimiter(_localFlowField, _localMesh, _localMetrics, iface, PrimitiveVarGrads,borderElem);
    std::vector<double> limiters(12);
	std::fill(limiters.begin(), limiters.end(), 1.0);


    Vector3<double> r_l = faceCenter - _localMetrics.getCellCentroids()[leftElementId];

    double leftRho = _localFlowField.Getrho()[leftElementId] + Vector3<double>::dot(PrimitiveVarGrads[0],r_l)*limiters[0];
    double leftU = _localFlowField.GetU_Velocity()[leftElementId] + Vector3<double>::dot(PrimitiveVarGrads[1],r_l)*limiters[1];
    double leftV = _localFlowField.GetV_Velocity()[leftElementId] + Vector3<double>::dot(PrimitiveVarGrads[2],r_l)*limiters[2];
    double leftW = _localFlowField.GetW_Velocity()[leftElementId] + Vector3<double>::dot(PrimitiveVarGrads[3],r_l)*limiters[3];
    double leftH = _localFlowField.GetH()[leftElementId] + Vector3<double>::dot(PrimitiveVarGrads[4],r_l)*limiters[4];
    double leftP = _localFlowField.GetP()[leftElementId] + Vector3<double>::dot(PrimitiveVarGrads[5],r_l)*limiters[5];

    double rightRho;
    double rightU;
    double rightV;
    double rightW;
    double rightH;
    double rightP;

    Vector3<double> r_r = faceCenter - _localMetrics.getCellCentroids()[rightElementId];
    if(rightElementId < _localMesh.GetnElemTot()){

    rightRho = _localFlowField.Getrho()[rightElementId] + Vector3<double>::dot(PrimitiveVarGrads[6],r_r)*limiters[6];
    rightU = _localFlowField.GetU_Velocity()[rightElementId] + Vector3<double>::dot(PrimitiveVarGrads[7],r_r)*limiters[7];
    rightV = _localFlowField.GetV_Velocity()[rightElementId] + Vector3<double>::dot(PrimitiveVarGrads[8],r_r)*limiters[8];
    rightW = _localFlowField.GetW_Velocity()[rightElementId] + Vector3<double>::dot(PrimitiveVarGrads[9],r_r)*limiters[9];
    rightH = _localFlowField.GetH()[rightElementId] + Vector3<double>::dot(PrimitiveVarGrads[10],r_r)*limiters[10];
    rightP = _localFlowField.GetP()[rightElementId] + Vector3<double>::dot(PrimitiveVarGrads[11],r_r)*limiters[11];
	}
	else{
        rightRho = _localFlowField.Getrho()[rightElementId];
        rightU = _localFlowField.GetU_Velocity()[rightElementId];
        rightV = _localFlowField.GetV_Velocity()[rightElementId];
        rightW = _localFlowField.GetW_Velocity()[rightElementId];
        rightH = _localFlowField.GetH()[rightElementId];
        rightP = _localFlowField.GetP()[rightElementId];
	}


//	if(leftElementId==0 && rightElementId==110){
//        printf("distrance To left : %.5f %.5f %.5f\n", r_l.x,r_l.y,r_l.z);
//        printf("distrance To right : %.5f %.5f %.5f\n", r_r.x,r_r.y,r_r.z);
//		printf("------\n");
//		for(int i=0;i<12;i++){
//
//			printf("PrimivteVar %d : %.5f %.5f %.5f \n",i,PrimitiveVarGrads[i].x,PrimitiveVarGrads[i].y,PrimitiveVarGrads[i].z);
//		}
//        printf("------\n");
//	}

    //get variables from FlowField
    double gamma = _localFlowField.getgamma_ref();


    double sqrtLeftRho = sqrt(leftRho);
    double sqrtRightRho = sqrt(rightRho);

    //calculate variables for flowfield
    double rhoTilde = sqrt(leftRho * rightRho);

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

std::vector<E3D::Vector3<double>> E3D::Solver::GreenGaussGradient(E3D::Solver::FlowField &Flowfield,
                                                                  const E3D::Mesh<E3D::Parser::MeshPartition> &Mesh,
                                                                  const E3D::Metrics &Metrics,
                                                                  const int iface,
                                                                  bool borderElem) {

    int *ptrFace2Elems = Mesh.GetFace2ElementID(iface);

    // Contains gradient of variables in the following order : rho,u,v,w,H,p .
    // First half is for the left side (6 values), and the second half for the right side (6 values)
    std::vector<E3D::Vector3<double>> GradPrimitiveVariables(12);
    int globalCounter = 0;
    // Computing Gradient with Green Gauss Method
    for (int i = 0; i < 2; i++) {
        int iElem = ptrFace2Elems[i];

        // Find Central element primitive Variables
        double cellVolume = Metrics.getCellVolumes()[iElem];
        std::array<double, 6> ThisElemPrimVar{
                Flowfield.Getrho()[iElem],
                Flowfield.GetU_Velocity()[iElem],
                Flowfield.GetV_Velocity()[iElem],
                Flowfield.GetW_Velocity()[iElem],
                Flowfield.GetH()[iElem],
                Flowfield.GetP()[iElem],
        };

        int nbSurrElems;
        int *ptrElemToFace = Mesh.GetElement2FaceID(iElem, nbSurrElems);

        std::array<Vector3<double>, 6> Grads{Vector3<double>(0, 0, 0),
                                             Vector3<double>(0, 0, 0),
                                             Vector3<double>(0, 0, 0),
                                             Vector3<double>(0, 0, 0),
                                             Vector3<double>(0, 0, 0),
                                             Vector3<double>(0, 0, 0)};


        for (int j = 0; j < nbSurrElems; j++) {
            int face = ptrElemToFace[j];
            int *ptrFace2ElemsGradient = Mesh.GetFace2ElementID(face);
            int AdjacentElem;
            iElem == ptrFace2ElemsGradient[0] ? AdjacentElem = ptrFace2ElemsGradient[1] : AdjacentElem = ptrFace2ElemsGradient[0];

            Vector3<double> UnitNormal = Metrics.getFaceNormalsUnit()[face];
            if (AdjacentElem > iElem) {
                UnitNormal *= -1;
            }
            double faceSurface = Metrics.getFaceSurfaces()[face];

            std::array<double, 6> AdjElemPrimVar{
                    Flowfield.Getrho()[AdjacentElem],
                    Flowfield.GetU_Velocity()[AdjacentElem],
                    Flowfield.GetV_Velocity()[AdjacentElem],
                    Flowfield.GetW_Velocity()[AdjacentElem],
                    Flowfield.GetH()[AdjacentElem],
                    Flowfield.GetP()[AdjacentElem],
            };


            for (int VarIdx = 0; VarIdx < 6; VarIdx++) {
                Grads[VarIdx] += UnitNormal * (ThisElemPrimVar[VarIdx] + AdjElemPrimVar[VarIdx]) * faceSurface * 0.5;
            }
        }

        for (int VarIdx = 0; VarIdx < 6; VarIdx++) {
            GradPrimitiveVariables[globalCounter] = Grads[VarIdx] / cellVolume;
            globalCounter++;
        }
        if (borderElem) {
            break;
        }
    }
    return GradPrimitiveVariables;
}

std::vector<double> E3D::Solver::VenkatakrishnanLimiter(E3D::Solver::FlowField &Flowfield,
                                                        const E3D::Mesh<E3D::Parser::MeshPartition> &Mesh,
                                                        const E3D::Metrics &Metrics,
                                                        const int iface,
                                                        const std::vector<Vector3<double>> &grads,
                                                        bool borderElem) {
    double K = 0.5;

    int nbEvaluatedCells;
    borderElem ? nbEvaluatedCells = 1 : nbEvaluatedCells = 2;

    int *ptrFace2Elems = Mesh.GetFace2ElementID(iface);

    // Contains Max, minimum values and cell values of primitives variables in the following order :
    // Lrho max, Lrho min, Lrho Lu max, Lu min, Lu,  Lv max,Lv min, Lv, lw max, LW min, .... Lp min
    // Then same thing again for the right side cell
    std::vector<double> MaxMinPrimVars;

    for (int i = 0; i < nbEvaluatedCells; i++) {
        std::array<double,6> MaxPrimVariables;
        std::array<double,6> MinPrimVariables;
        int iElem = ptrFace2Elems[i];

        std::array<double, 6> ThisElemPrimVar{
                Flowfield.Getrho()[iElem],
                Flowfield.GetU_Velocity()[iElem],
                Flowfield.GetV_Velocity()[iElem],
                Flowfield.GetW_Velocity()[iElem],
                Flowfield.GetH()[iElem],
                Flowfield.GetP()[iElem],
        };

        MaxPrimVariables = ThisElemPrimVar;
        MinPrimVariables = ThisElemPrimVar;

        int nbSurrElems;
        int *ptrElemToFace = Mesh.GetElement2FaceID(iElem, nbSurrElems);

        for (int j = 0; j < nbSurrElems; j++) {
            int face = ptrElemToFace[j];
            int *ptrFace2ElemsGradient = Mesh.GetFace2ElementID(face);
            int AdjacentElem;
            iElem == ptrFace2ElemsGradient[0] ? AdjacentElem = ptrFace2ElemsGradient[1] : AdjacentElem = ptrFace2ElemsGradient[0];

            std::array<double, 6> AdjElemPrimVar{
                    Flowfield.Getrho()[AdjacentElem],
                    Flowfield.GetU_Velocity()[AdjacentElem],
                    Flowfield.GetV_Velocity()[AdjacentElem],
                    Flowfield.GetW_Velocity()[AdjacentElem],
                    Flowfield.GetH()[AdjacentElem],
                    Flowfield.GetP()[AdjacentElem],
            };


            for(int CellIdx=0; CellIdx < 6 ; CellIdx++){
                if(AdjElemPrimVar[i] > MaxPrimVariables[i]){
                    MaxPrimVariables[i] = AdjElemPrimVar[i];
                }
                else if (AdjElemPrimVar[i] < MinPrimVariables[i]){
                    MinPrimVariables[i] =AdjElemPrimVar[i];
                }
            }

        }

        for(int CellIdx=0; CellIdx < 6 ; CellIdx++){
            MaxMinPrimVars.push_back(MaxPrimVariables[i]);
            MaxMinPrimVars.push_back(MinPrimVariables[i]);
            MaxMinPrimVars.push_back(ThisElemPrimVar[i]);
        }
    }


    Vector3<double> faceCenter = Metrics.getFaceCenters()[iface];
    int globalCounter = 0;
    std::vector<double> limiters;
    for (int i = 0; i < nbEvaluatedCells; i++) {
        int iElem = ptrFace2Elems[i];

        auto r_vector = faceCenter - Metrics.getCellCentroids()[iElem];
        for (int primVarIdx=0; primVarIdx < 6 ;primVarIdx++){
            double delta2 = Vector3<double>::dot(grads[globalCounter],r_vector) ;
            delta2 > 0 ? delta2 = std::abs(delta2) + std::numeric_limits<double>::epsilon()
                       : delta2 = (std::abs(delta2) + std::numeric_limits<double>::epsilon())*-1;
            double limiter;
            if (delta2 == 0){
                limiter = 1;
            }
            else if(delta2 > 0){
                double epsilon_sqrd = std::pow(K * std::pow(Metrics.getCellVolumes()[iElem], 1/3),3);
                double delta1_max = MaxMinPrimVars[globalCounter*3] - MaxMinPrimVars[globalCounter*3+2];

                limiter = (1/delta2) * ((std::pow(delta1_max,2)+epsilon_sqrd)*delta2 + 2*std::pow(delta2,2)*delta1_max)
                          / (std::pow(delta1_max,2) + 2*std::pow(delta2,2)+ delta1_max*delta2 + epsilon_sqrd);
            }
            else if(delta2 < 0){
                double epsilon_sqrd = std::pow(K * std::pow(Metrics.getCellVolumes()[iElem], 1/3),3);
                double delta1_min = MaxMinPrimVars[globalCounter*3+1] - MaxMinPrimVars[globalCounter*3+2];
                limiter = (1/delta2) * ((std::pow(delta1_min,2)+epsilon_sqrd)*delta2 + 2*std::pow(delta2,2)*delta1_min)
                          / (std::pow(delta1_min,2) + 2*std::pow(delta2,2)+ delta1_min*delta2 + epsilon_sqrd);
            }
            limiters.push_back(limiter);
            globalCounter++;
        }

    }
    return limiters;

}