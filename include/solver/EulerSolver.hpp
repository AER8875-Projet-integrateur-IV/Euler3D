//
// Created by amin on 3/1/21.
//
#pragma once
#include "mesh/Metrics.hpp"
#include "solver/AeroCoefficients.hpp"
#include "solver/ConvectiveFluxesSchemes.h"
#include "solver/FlowField.hpp"
#include "solver/PhysicalBC.h"
#include "solver/ResidualsFile.hpp"
#include "solver/TimeIntegration.h"
#include <fstream>
#include <utils/Vector3.h>

namespace E3D::Solver {

	class EulerSolver {
	public:
		EulerSolver(FlowField &localFlowField,
		            const E3D::Parallel::MPIHandler &e3d_mpi,
		            const E3D::Mesh<E3D::Parser::MeshPartition> &localMesh,
		            const E3D::Parser::SimConfig &config,
		            const E3D::Metrics &);

		void Run();

		void WriteSummary();

	private:
		/**
         * @brief Computes boundary faces fluxes depending on :
         * - farfield state (Subsonic/Supersonic, Inflow/Outflow)
         * - Wall/airfoil (V.n = 0)
         * - Mpi Ghost Elements (exchange primitive variables between partitions)
         */
		void updateBC();

		/**
		 * @brief Compute maximum ResidualVar mean square
		 */
		double computeRMS();

		/**
		 * @brief Compute forces around airfoil/wall
		 */
		void computeForces();

		/**
		 * @brief compute aerodynamic coefficients (CL,CD,CP,Cm) with computed forces
		 */
		void updateAerodynamicCoefficients();

		/**
		 * @brief compute local time step of cell
		 */
		void updateDeltaTime();

		/**
		 * @brief compute convective fluxes with Roe scheme
		 */
		void computeResidual();

		/**
         * @brief Compute DeltaW with Explicit/Implicit Euler or Runge-Kutta
         */
		void TimeIntegration();

		/**
         * @brief UpdateW in localFlowField
         */
		void updateW();

		/**
		 * @brief creates sorted sort cells to accelerate search (with binary search)
		 */
		void sortGhostCells();

		void resetResiduals();

		/**
		 * @brief Print current iteration info to console and to file
		 *
		 * @param iterationwallTime Iteration time
		 * @param sumError maximal current RMS error
		 */
		void PrintInfo(double iterationwallTime, double sumError);

		/**
		 * @brief Print Cp and centroid coordinates to file in output dir
		 *
		 */
		void PrintCp();

		/**
		 * @brief check if congervence is obtained
		 *
		 */
		bool ConvergenceCriteria(double CL_old, double CD_old, double CM_old, double, double, double);

		void BroadCastCoeffs(std::vector<double> &);

		void RungeKutta();
		void EulerExplicit();

		// function to use for time integration
		void (EulerSolver::*_timeIntegrator)();

		std::vector<E3D::Solver::ResidualVar> _residuals;
		E3D::Vector3<double> _forces;
		E3D::Vector3<double> _centrePressure;
		E3D::Vector3<double> _forceCoefficients;
		E3D::Vector3<double> _momentCoefficients;
		E3D::Vector3<double> _referencePoint;

		// {cl axis, cl orientation, cd axis, cd orientation, cm axis, cm orientation, }
		std::vector<std::pair<int, int>> _coeffOrientation;
		int _samplePeriod;

		std::vector<double> _deltaT;
		std::vector<ConservativeVar> _deltaW;
		// std::vector<double> _cp;
		E3D::Solver::AeroCoefficients _coeff;

		FlowField &_localFlowField;
		const E3D::Parallel::MPIHandler &_e3d_mpi;
		const E3D::Mesh<E3D::Parser::MeshPartition> &_localMesh;
		const E3D::Parser::SimConfig &_config;
		const E3D::Metrics &_localMetrics;
		std::filesystem::path _outputDir;
		ResidualsFile _residualFile;

		const std::vector<std::pair<int, std::vector<int>>> _PairsMpiGhostCellIDs = _localMesh.VectorGetMPIGhostCellsIDs();
		const std::vector<int> MPIghostCellElems = _localMesh.GetMPIGhostCellsIDs();
		const std::vector<int> _SymmetryGhostCellIDs = _localMesh.GetSymmetryGhostCellsIDs();
		const std::vector<int> _FarfieldGhostCellIDs = _localMesh.GetFarfieldGhostCellsIDs();
		const std::vector<int> _WallGhostCellIDs = _localMesh.GetWallGhostCellsIDs();
		std::vector<int> _sortedMPIGhostCellIDs;
		std::vector<int> _sortedFarfieldGhostCellIDs;
		std::vector<int> _sortedWallGhostCellIDs;
		std::vector<int> _sortedSymmetryGhostCellIDs;


		int _nbInteration = 0;
		double _rms = 10;

		double _CL = 0;
		double _CD = 0;
		double _CM = 0;
	};
}// namespace E3D::Solver
