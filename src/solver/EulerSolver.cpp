//
// Created by amin on 3/1/21.
//

#include "solver/EulerSolver.hpp"
#include "utils/SolverPrint.hpp"
#include "utils/Vector3.h"
#include <math.h>

using namespace E3D;

Solver::EulerSolver::EulerSolver(FlowField &localFlowField,
                                 const E3D::Parallel::MPIHandler &e3d_mpi,
                                 const E3D::Mesh<E3D::Parser::MeshPartition> &localMesh,
                                 const E3D::Parser::SimConfig &config,
                                 const E3D::Metrics &localMetrics)

    : _localFlowField(localFlowField), _e3d_mpi(e3d_mpi), _localMesh(localMesh), _config(config), _localMetrics(localMetrics) {

	if (e3d_mpi.getRankID() == 0) {
		std::cout << "\n\n"
		          << std::string(24, '#') << "  Starting Solving Process !  " << std::string(24, '#') << "\n\n";
		// Open file to write residual
		residualFile.open("resdiual_" + _config.getTecplotFile());
		residualFile << "Rho residual \n";
	}
}

void Solver::EulerSolver::Run() {

	if (_e3d_mpi.getRankID() == 0) {
		E3D::Solver::printHeader();
	}
	sortGhostCells();

	// resize residual, deltaT and deltaW vectors
	_residuals.resize(_localMesh.GetnElemTot(), ResidualVar(0.0,
	                                                        0.0,
	                                                        0.0,
	                                                        0.0,
	                                                        0.0));
	_deltaT.resize(_localMesh.GetnElemTot(), 0);
	_deltaW.resize(_localMesh.GetnElemTot(), ConservativeVar(0.0,
	                                                         0.0,
	                                                         0.0,
	                                                         0.0,
	                                                         0.0));


	while (_nbInteration < _config.getMaxNumberIterations()) {
		resetResiduals();

		double iterationBeginTimer = MPI_Wtime();


		// loop Through Ghost cells (Boundary Cells)
		updateBC();
		computeResidual();


		//TODO Exchange max RMS between partition;
		double error = computeRMS();
		double _sumerrors = 0;
		MPI_Allreduce(&error, &_sumerrors, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
		double _maximumDomainRms = std::sqrt(_sumerrors / _localFlowField.getTotalDomainCounts());
		if (_maximumDomainRms < _config.getMinResidual()) {
			if (_e3d_mpi.getRankID() == 0) {
				double iterationEndTimer = MPI_Wtime();
				double iterationwallTime = iterationEndTimer - iterationBeginTimer;
				E3D::Solver::PrintSolverIteration(_CL, _CD, _maximumDomainRms, iterationwallTime, _nbInteration);
				residualFile << _maximumDomainRms << "\n";
				residualFile.close();
			}
			break;
		}

		updateDeltaTime();
		TimeIntegration();
		updateW();


		_nbInteration += 1;

		if (_e3d_mpi.getRankID() == 0) {
			residualFile << _maximumDomainRms << "\n";
			if (_nbInteration % 20 == 0) {
				double _maximumDomainRms = std::sqrt(_sumerrors / _localFlowField.getTotalDomainCounts());
				double iterationEndTimer = MPI_Wtime();
				double iterationwallTime = iterationEndTimer - iterationBeginTimer;
				E3D::Solver::PrintSolverIteration(_CL, _CD, _maximumDomainRms, iterationwallTime, _nbInteration);
			}
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}

	// If loop exited because of reaching maximum number of iteration, print this message
	if (_e3d_mpi.getRankID() == 0) {

		if (_config.getMaxNumberIterations() <= _nbInteration) {
			std::cout << "Solution not converged :  exceeded maximum number of iterations !";
		} else {
			std::cout << "Solution converged OR a NAN encountred !";
		}
	}
}


// Update Farfield, Wall, Symmetry and MPI ghost cell primitive values
void Solver::EulerSolver::updateBC() {


	// Update Farfield
	for (size_t GhostID = 0; GhostID < _FarfieldGhostCellIDs.size(); GhostID++) {

		int GhostIdx = _FarfieldGhostCellIDs[GhostID];
		int FaceGhostIdx = _localMesh.GetFarfieldAdjacentFaceIDs()[GhostID];
		int InteriorGhostIdx = _localMesh.GetFarfieldAdjacentToGhostCellsIDs()[GhostID];

		double V = _localFlowField.GetU_Velocity()[GhostIdx] * _localMetrics.getFaceNormalsUnit()[FaceGhostIdx].x +
		           _localFlowField.GetV_Velocity()[GhostIdx] * _localMetrics.getFaceNormalsUnit()[FaceGhostIdx].y +
		           _localFlowField.GetW_Velocity()[GhostIdx] * _localMetrics.getFaceNormalsUnit()[FaceGhostIdx].z;

		double M = _localFlowField.GetMach()[GhostIdx];

		if (M >= 1.0 && V > 0.0) {
			Solver::BC::FarfieldSupersonicOutflow(_localFlowField, GhostIdx, InteriorGhostIdx);
		} else if (M < 1.0 && V > 0.0) {
			Solver::BC::FarfieldSubsonicOutflow(_localFlowField, _localMetrics, GhostIdx, InteriorGhostIdx, FaceGhostIdx);
		} else if (M >= 1.0 && V <= 0.0) {
			Solver::BC::FarfieldSupersonicInflow(_localFlowField, GhostIdx);

		} else if (M < 1.0 && V <= 0.0) {
			Solver::BC::FarfieldSubsonicInflow(_localFlowField, _localMetrics, GhostIdx, InteriorGhostIdx, FaceGhostIdx);
		}
	}


	// Update Wall
	for (size_t GhostID = 0; GhostID < _WallGhostCellIDs.size(); GhostID++) {
		int GhostIdx = _WallGhostCellIDs[GhostID];
		int FaceGhostIdx = _localMesh.GetWallAdjacentFaceIDs()[GhostID];
		int InteriorGhostIdx = _localMesh.GetWallAdjacentGhostCellIDs()[GhostID];

		Solver::BC::Wall(_localFlowField, _localMetrics, GhostIdx, InteriorGhostIdx, FaceGhostIdx);
	}


	// Update Symmetry
	for (size_t GhostID = 0; GhostID < _SymmetryGhostCellIDs.size(); GhostID++) {

		int GhostIdx = _SymmetryGhostCellIDs[GhostID];
		int InteriorGhostIdx = _localMesh.GetSymmetryAdjacentGhostCellIDs()[GhostID];

		Solver::BC::Symmetry(_localFlowField, GhostIdx, InteriorGhostIdx);
	}
	// Update MPI

	_e3d_mpi.updateFlowField(_localFlowField);
}


void Solver::EulerSolver::computeResidual() {

	//loop Thougth internal faces
	for (int IfaceID = 0; IfaceID < _localMesh.GetnFaceInt(); IfaceID++) {

		int *ptr = _localMesh.GetFace2ElementID(IfaceID);
		int element1 = ptr[0];
		int element2 = ptr[1];

		ResidualVar residu = Solver::Roe(_localFlowField, _localMesh, _localMetrics, IfaceID, false);


		double surfaceArea = _localMetrics.getFaceSurfaces()[IfaceID];
		_residuals[element1] += residu * surfaceArea;
		_residuals[element2] -= residu * surfaceArea;
	}


	// loop through external faces
	for (int EfaceID = _localMesh.GetnFaceInt(); EfaceID < _localMesh.GetnFace(); EfaceID++) {

		int *ptr = _localMesh.GetFace2ElementID(EfaceID);
		int element1 = ptr[0];
		int element2 = ptr[1];
		double surfaceArea = _localMetrics.getFaceSurfaces()[EfaceID];


		//If Wall
		if (std::binary_search(_sortedWallGhostCellIDs.begin(), _sortedWallGhostCellIDs.end(), element2)) {
			double composant1 = _localMetrics.getFaceNormalsUnit()[EfaceID].x * _localFlowField.GetP()[element2];
			double composant2 = _localMetrics.getFaceNormalsUnit()[EfaceID].y * _localFlowField.GetP()[element2];
			double composant3 = _localMetrics.getFaceNormalsUnit()[EfaceID].z * _localFlowField.GetP()[element2];

			ResidualVar residu = {0, composant1, composant2, composant3, 0};
			_residuals[element1] += residu * surfaceArea;

		}


		// If MPI or Symmetry
		else if (std::binary_search(_sortedMPIGhostCellIDs.begin(), _sortedMPIGhostCellIDs.end(), element2) || std::binary_search(_sortedSymmetryGhostCellIDs.begin(), _sortedSymmetryGhostCellIDs.end(), element2)) {
			ResidualVar residu = Solver::Roe(_localFlowField, _localMesh, _localMetrics, EfaceID, true);
			_residuals[element1] += residu * surfaceArea;
		}

		// if farfield
		else {
            double V = _localFlowField.GetU_Velocity()[element2] * _localMetrics.getFaceNormalsUnit()[EfaceID].x +
                       _localFlowField.GetV_Velocity()[element2] * _localMetrics.getFaceNormalsUnit()[EfaceID].y +
                       _localFlowField.GetW_Velocity()[element2] * _localMetrics.getFaceNormalsUnit()[EfaceID].z;

			ResidualVar residu = Solver::Fc(_localFlowField, _localMetrics, element2, EfaceID, V);
			_residuals[element1] += residu * surfaceArea;
		}
	}
}

void Solver::EulerSolver::updateDeltaTime() {

	for (int ielem = 0; ielem < _localMesh.GetnElemTot(); ielem++) {
		_deltaT[ielem] = Solver::TimeStepMethod2(_localFlowField, _localMesh, _localMetrics, ielem);
	}
}


void Solver::EulerSolver::TimeIntegration() {
	for (int ielem = 0; ielem < _localMesh.GetnElemTot(); ielem++) {

		_deltaW[ielem] = Solver::ExplicitEuler(_residuals[ielem], _deltaT[ielem], _localMetrics, ielem);
	}
}

void Solver::EulerSolver::updateW() {

	_localFlowField.Update(_deltaW, MPIghostCellElems, _localMesh.getMPIadjacentToGhostCellIDs());
}

double Solver::EulerSolver::computeRMS() {
	double erreur = 0.0;
	//TODO LOOP THROUGH RESIDUALS OR DW ?
	for (const auto &residu : _residuals) {
		erreur += std::pow(residu.m_rhoV_residual, 2);
	}
	return erreur;
}

void Solver::EulerSolver::resetResiduals() {
	for (auto &residu : _residuals) {
		residu.reset();
	}
}


void Solver::EulerSolver::sortGhostCells() {
	_sortedMPIGhostCellIDs = MPIghostCellElems;
	_sortedFarfieldGhostCellIDs = _FarfieldGhostCellIDs;
	_sortedSymmetryGhostCellIDs = _SymmetryGhostCellIDs;
	_sortedWallGhostCellIDs = _WallGhostCellIDs;
	std::sort(_sortedWallGhostCellIDs.begin(), _sortedWallGhostCellIDs.end());
	std::sort(_sortedMPIGhostCellIDs.begin(), _sortedMPIGhostCellIDs.end());
	std::sort(_sortedFarfieldGhostCellIDs.begin(), _sortedFarfieldGhostCellIDs.end());
	std::sort(_sortedSymmetryGhostCellIDs.begin(), _sortedSymmetryGhostCellIDs.end());
}
