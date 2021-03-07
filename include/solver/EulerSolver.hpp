//
// Created by amin on 3/1/21.
//
#pragma once
#include "solver/FlowField.hpp"
#include <utils/Vector3.h>
#include "solver/PhysicalBC.h"
#include "mesh/Metrics.hpp"
#include "solver/ConvectiveFluxesSchemes.h"
#include "solver/TimeIntegration.h"


namespace E3D::Solver {

	class EulerSolver {
	public:
		EulerSolver(FlowField& localFlowField,
		            const E3D::Parallel::MPIHandler& e3d_mpi,
		            const E3D::Mesh<E3D::Parser::MeshPartition>& localMesh,
		            const E3D::Parser::SimConfig& config,
		            const E3D::Metrics&);

		void Run();

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

        void resetResiduals();

        std::vector<E3D::Vector3<double>> _forces;
		std::vector<E3D::Solver::ResidualVar> _residuals;
		std::vector<double> _deltaT;
		std::vector<ConservativeVar> _deltaW;
		std::vector<double> _cp;



        FlowField& _localFlowField;
        const E3D::Parallel::MPIHandler& _e3d_mpi;
        const E3D::Mesh<E3D::Parser::MeshPartition>& _localMesh;
        const E3D::Parser::SimConfig& _config;
		const E3D::Metrics& _localMetrics;

        const std::vector<std::pair<int, std::vector<int>>> _PairsMpiGhostCellIDs = _localMesh.VectorGetMPIGhostCellsIDs();
		const std::vector<int> MPIghostCellElems = _localMesh.GetMPIGhostCellsIDs();
        const std::vector<int> _SymmetryGhostCellIDs = _localMesh.GetSymmetryGhostCellsIDs();
        const std::vector<int> _FarfieldGhostCellIDs = _localMesh.GetFarfieldGhostCellsIDs();
        const std::vector<int> _WallGhostCellIDs = _localMesh.GetWallGhostCellsIDs();
		std::vector<int> InteriorandMPIandSymmetryElemsIDs;

		int _nbInteration=0;
        double _maximumDomainRms =10;

		double _CL=0;
		double _CD=0;
		double _CM=0;


	};
}// namespace E3D::Solver
