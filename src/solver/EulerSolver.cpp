//
// Created by amin on 3/1/21.
//
#include "solver/EulerSolver.hpp"
#include "solver/ResidualsFile.hpp"
#include "toml++/toml.h"
#include "utils/SolverPrint.hpp"
#include "utils/Vector3.h"
#include <filesystem>
#include <iomanip>
#include <math.h>

// #include "sgs/cal_Q.h"
// #include "sgs/muscl.h"
// #include "sgs/parameters.h"
// #include "sgs/setup.h"
// #include "sgs/utils.h"

using namespace E3D;

Solver::EulerSolver::EulerSolver(FlowField &localFlowField,
                                 const E3D::Parallel::MPIHandler &e3d_mpi,
                                 const E3D::Mesh<E3D::Parser::MeshPartition> &localMesh,
                                 const E3D::Parser::SimConfig &config,
                                 const E3D::Metrics &localMetrics)

    : _localFlowField(localFlowField), _e3d_mpi(e3d_mpi), _localMesh(localMesh), _config(config), _localMetrics(localMetrics) {

	_samplePeriod = config.getSamplingPeriod();
	_outputDir = config.GetoutputDir();
	if (e3d_mpi.getRankID() == 0) {
		std::cout << "\n\n"
		          << std::string(24, '#') << "  Starting Solving Process !  " << std::string(24, '#') << "\n\n";
		// Open file to write residual
		new (&_residualFile) ResidualsFile(_outputDir / "residuals.dat");
	}
	E3D::Vector3<double> uInf(localFlowField.getu_inf(), localFlowField.getv_inf(), localFlowField.getw_inf());
	_coeffOrientation = std::vector<std::pair<int, int>>{config.getMeshOrientationCL(),
	                                                     config.getMeshOrientationCD(),
	                                                     config.getMeshOrientationCM()};
	_coeff = E3D::Solver::AeroCoefficients(localFlowField.getp_inf(),
	                                       localFlowField.getrho_inf(),
	                                       uInf,
	                                       localMesh,
	                                       localMetrics,
	                                       config.getMeshRefPoint(),
	                                       config.getSref(),
	                                       config.getCref());

	// define time integration method
	if (_config.getTemporalScheme() == Parser::SimConfig::TemporalScheme::RK5) {
		_timeIntegrator = &Solver::EulerSolver::RungeKutta;
	} else if (_config.getTemporalScheme() == Parser::SimConfig::TemporalScheme::IMPLICIT_EULER) {
		_timeIntegrator = &Solver::EulerSolver::EulerImplicit;
	} else {
		_timeIntegrator = &Solver::EulerSolver::EulerExplicit;
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

	int convergenceCounter = 0;
	bool criteria = false;
	while (_nbInteration < _config.getMaxNumberIterations()) {

		resetResiduals();
		double iterationBeginTimer = MPI_Wtime();


		// loop Through Ghost cells (Boundary Cells)
		updateBC();
		computeResidual();
		if (_config.getResidualSmoothing()) {
			smoothResiduals();
		}

		(this->*_timeIntegrator)();

		_nbInteration += 1;


		if (_nbInteration % _samplePeriod == 0) {//_samplePeriod
            double error = computeRMS();
            double _sumerrors = 0.0;
            MPI_Allreduce(&error, &_sumerrors, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            double _maximumDomainRms = std::sqrt(_sumerrors / _localFlowField.getTotalDomainCounts());


			// Broadcast coeffs from partition 0 to other partitions
			std::vector<double> Oldglobalcoeffs{_CL, _CD, _CM};
			BroadCastCoeffs(Oldglobalcoeffs);

			updateAerodynamicCoefficients();


			if (_e3d_mpi.getRankID() == 0) {
				double iterationEndTimer = MPI_Wtime();
				double iterationwallTime = iterationEndTimer - iterationBeginTimer;
				PrintInfo(iterationwallTime, _maximumDomainRms);
			}

			// Broadcast coeffs from partition 0 to other partitions
			std::vector<double> globalcoeffs{_CL, _CD, _CM};
			BroadCastCoeffs(globalcoeffs);

			criteria = ConvergenceCriteria(Oldglobalcoeffs[0],
			                               Oldglobalcoeffs[1],
			                               Oldglobalcoeffs[2],
			                               globalcoeffs[0],
			                               globalcoeffs[1],
			                               globalcoeffs[2]);

            //TODO Exchange max RMS between partition;

            if (_maximumDomainRms < _config.getMinResidual()) {
                if (_e3d_mpi.getRankID() == 0) {
                    double iterationEndTimer = MPI_Wtime();
                    double iterationwallTime = iterationEndTimer - iterationBeginTimer;
                    PrintInfo(iterationwallTime, _maximumDomainRms);
                }
                break;
            }

			if (criteria) {
				convergenceCounter++;
			} else {
				convergenceCounter = 0;
			}
			if (convergenceCounter >= 2) {
				if (_e3d_mpi.getRankID() == 0) {
					printf("Coefficients have converged to %.2E\n", _config.getMinAeroCoeffError());
				}
				break;
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

void Solver::EulerSolver::WriteSummary(long solverTimer, long totalTimer) {
	PrintCp();
	auto sumWallVectors = _coeff.GetSumFaceVectors();
	sumWallVectors = _e3d_mpi.UpdateAerodynamicCoefficients(sumWallVectors);

	if (_e3d_mpi.getRankID() == 0) {
		double TimeCore_p_IterNElem = static_cast<double>(solverTimer * _e3d_mpi.getPoolSize()) /
		                              static_cast<double>(_nbInteration * _localMesh.GetNElemGlobalMesh());
		// Write summary in TOML format
		auto tbl = toml::table{{{"Mesh", toml::table{{{"Wall_Face_Vector_Sum", toml::array{sumWallVectors.x, sumWallVectors.y, sumWallVectors.z}},
		                                              {"NElem", _localMesh.GetNElemGlobalMesh()}}}},
		                        {"Coefficients", toml::table{{
		                                                 {"Force_Coeff", toml::array{_forceCoefficients.x, _forceCoefficients.y, _forceCoefficients.z}},
		                                                 {"Moment_Coeff", toml::array{_momentCoefficients.x, _momentCoefficients.y, _momentCoefficients.z}},
		                                                 {"CL", _CL},
		                                                 {"CD", _CD},
		                                                 {"CM", _CM},
		                                         }}},
		                        {"TotalSolverTime_ms", totalTimer},
		                        {"SolverLoopTime_ms", solverTimer},
		                        {"Iterations", _nbInteration},
		                        {"TimeCore_p_IterNElem_ms", TimeCore_p_IterNElem},
		                        {"Ncores", _e3d_mpi.getPoolSize()}}};
		std::filesystem::path path = _outputDir / "summary.txt";
		std::ofstream file(path);
		file << tbl << std::endl;
		file.close();
		std::cout << "\nSummary printed at: " << path << std::endl;
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
		int FaceGhostIdx = _localMesh.GetSymmetryAdjacentFaceIDs()[GhostID];
		Solver::BC::Symmetry(_localFlowField, _localMetrics, GhostIdx, InteriorGhostIdx, FaceGhostIdx);
	}
	// Update MPI

	_e3d_mpi.updateFlowField(_localFlowField);
}

void Solver::EulerSolver::PrintInfo(double iterationwallTime, double rms) {
	_CL = _forceCoefficients[_coeffOrientation[0].first] * _coeffOrientation[0].second;
	_CD = _forceCoefficients[_coeffOrientation[1].first] * _coeffOrientation[1].second;
	_CM = _momentCoefficients[_coeffOrientation[2].first] * _coeffOrientation[2].second;
	_residualFile.Update(rms, _forceCoefficients, _momentCoefficients, _nbInteration);
	E3D::Solver::PrintSolverIteration(_CL, _CD, _CM, rms, iterationwallTime, _nbInteration);
}

bool Solver::EulerSolver::ConvergenceCriteria(double CL_old,
                                              double CD_old,
                                              double CM_old,
                                              double CL_new,
                                              double CD_new,
                                              double CM_new) {

	double residuCL = std::abs(CL_old - CL_new);//erreur absolue
	double residuCD = std::abs(CD_old - CD_new);
	double residuCM = std::abs(CM_old - CM_new);
	double tolerance = _config.getMinAeroCoeffError();
	bool sortie;
	if ((residuCL < tolerance && residuCD < tolerance && residuCM < tolerance)) {
		sortie = true;
	} else {
		sortie = false;
	}
	return sortie;
}


void Solver::EulerSolver::PrintCp() {
	std::ofstream file(_outputDir / ("cp_part_" + std::to_string(_e3d_mpi.getRankID()) + ".dat"));
	file << std::setw(20) << "Cp"
	     << std::setw(20) << "x"
	     << std::setw(20) << "y"
	     << std::setw(20) << "z"
	     << std::endl;

	std::vector<E3D::Vector3<double>> centroids = _coeff.GetCentroids();
	std::vector<double> cp = _coeff.GetCp();
	for (int i = 0; i < centroids.size(); i++) {
		E3D::Vector3<double> &cent = centroids[i];
		file << std::scientific << std::setprecision(10) << std::setw(20) << cp[i]
		     << std::scientific << std::setprecision(10) << std::setw(20) << cent.x
		     << std::scientific << std::setprecision(10) << std::setw(20) << cent.y
		     << std::scientific << std::setprecision(10) << std::setw(20) << cent.z
		     << std::endl;
	}
	file.close();
}

void Solver::EulerSolver::computeResidual() {

	//loop Thougth internal faces
	for (int IfaceID = 0; IfaceID < _localMesh.GetnFaceInt(); IfaceID++) {

		int *ptr = _localMesh.GetFace2ElementID(IfaceID);
		int element1 = ptr[0];
		int element2 = ptr[1];

		ResidualVar residu = Solver::Roe(_localFlowField, _localMesh, _localMetrics, IfaceID);


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
			ResidualVar residu = Solver::Roe(_localFlowField, _localMesh, _localMetrics, EfaceID);
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
void Solver::EulerSolver::updateAerodynamicCoefficients() {
	_coeff.Update(_localFlowField.GetP());
	_forceCoefficients = _coeff.GetForceCoeff();
	_forceCoefficients = _e3d_mpi.UpdateAerodynamicCoefficients(_forceCoefficients);
	_momentCoefficients = _coeff.GetMomentCoeff();
	_momentCoefficients = _e3d_mpi.UpdateAerodynamicCoefficients(_momentCoefficients);
}
void Solver::EulerSolver::BroadCastCoeffs(std::vector<double> &vec) {
	MPI_Bcast(&vec[0], 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void Solver::EulerSolver::EulerExplicit() {
	updateDeltaTime();
	TimeIntegration();
	updateW();
}

void Solver::EulerSolver::RungeKutta() {

	std::array<double, 4> RKcoefficients = {0.1263, 0.2375, 0.4414, 1.0};
	int ntotalElem = _localMesh.GetnElemTot();
	std::vector<ConservativeVar> RHS_W(ntotalElem);
	std::vector<ConservativeVar> W0(ntotalElem);
	for (int i = 0; i < ntotalElem; i++) {
		double rho = _localFlowField.Getrho()[i];
		W0[i].rho = rho;
		W0[i].rhoU = _localFlowField.GetU_Velocity()[i] * rho;
		W0[i].rhoV = _localFlowField.GetV_Velocity()[i] * rho;
		W0[i].rhoW = _localFlowField.GetW_Velocity()[i] * rho;
		W0[i].rhoE = _localFlowField.GetE()[i] * rho;
	}
	updateDeltaTime();

	for (int i = 0; i < ntotalElem; i++) {
		double volume = _localMetrics.getCellVolumes()[i];
		double dt = _deltaT[i];
		RHS_W[i].rho = 0.0533 * dt * _residuals[i].m_rhoV_residual / volume;
		RHS_W[i].rhoU = 0.0533 * dt * _residuals[i].m_rho_uV_residual / volume;
		RHS_W[i].rhoV = 0.0533 * dt * _residuals[i].m_rho_vV_residual / volume;
		RHS_W[i].rhoW = 0.0533 * dt * _residuals[i].m_rho_wV_residual / volume;
		RHS_W[i].rhoE = 0.0533 * dt * _residuals[i].m_rho_HV_residual / volume;
	}
	_localFlowField.updateWRungeKutta(RHS_W, W0);

	for (auto &alpha : RKcoefficients) {
		resetResiduals();
		updateBC();
		computeResidual();
		if (_config.getResidualSmoothing()) {
			smoothResiduals();
		}
		updateDeltaTime();

		for (int i = 0; i < ntotalElem; i++) {
			double volume = _localMetrics.getCellVolumes()[i];
			double dt = _deltaT[i];
			RHS_W[i].rho = alpha * dt * _residuals[i].m_rhoV_residual / volume;
			RHS_W[i].rhoU = alpha * dt * _residuals[i].m_rho_uV_residual / volume;
			RHS_W[i].rhoV = alpha * dt * _residuals[i].m_rho_vV_residual / volume;
			RHS_W[i].rhoW = alpha * dt * _residuals[i].m_rho_wV_residual / volume;
			RHS_W[i].rhoE = alpha * dt * _residuals[i].m_rho_HV_residual / volume;
		}
		_localFlowField.updateWRungeKutta(RHS_W, W0);
	}
}

void Solver::EulerSolver::EulerImplicit() {
	updateDeltaTime();//specific method for euler implicit, should be good for time method2

	// for (int i = 0; i < nstep; i++) {
	// 	cal_Q();
	// }
}

void Solver::EulerSolver::smoothResiduals() {


	const double epsilon = 0.5;
	auto original_residual = _residuals;
	std::vector<ResidualVar> last_residual(_localMesh.GetnElemTot());
	std::vector<ResidualVar> diff(_localMesh.GetnElemTot());
	double error;
	do {
		error = 0.0;

		last_residual = _residuals;
		for (int ielem = 0; ielem < _localMesh.GetnElemTot(); ielem++) {
			ResidualVar sumSurrResidual(0.0, 0.0, 0.0, 0.0, 0.0);
			int nSurrElems;
			int *SurrElems = _localMesh.GetElement2ElementID(ielem, nSurrElems);
			for (int i = 0; i < nSurrElems; i++) {
				if (SurrElems[i] < _localMesh.GetnElemTot()) {
					sumSurrResidual += (last_residual[SurrElems[i]] * epsilon);
				}
			}
			_residuals[ielem] = (original_residual[ielem] + sumSurrResidual) / (1 + nSurrElems * epsilon);
		}
		std::transform(_residuals.begin(), _residuals.end(), last_residual.begin(), diff.begin(), std::minus<ResidualVar>());

		for (auto &ResidualDiff : diff) {
			double maxRes = ResidualDiff.findMax();
			if (maxRes > error) {
				error = maxRes;
			}
		}

		diff.clear();
		last_residual.clear();
	} while (error > 1e-14);
}