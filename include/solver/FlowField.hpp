#pragma once

#include "mesh/Mesh.hpp"
#include "parallelization/MPIHandler.hpp"
#include "parser/SimConfig.hpp"
#include "solver/ConservativeVar.hpp"
#include <iostream>
#include <vector>

namespace E3D::Solver {

	class FlowField {

	public:
		FlowField(const E3D::Parser::SimConfig &config,
		          const E3D::Mesh<E3D::Parser::MeshPartition> &,
		          const E3D::Parallel::MPIHandler &);

		void PrintTest();

		void Update(std::vector<E3D::Solver::ConservativeVar> delW_vector);

		// Return rho (density) of problem
		inline const std::vector<double> &Getrho() const { return this->_rho; }

		// Return rhoU of problem
		inline const std::vector<double> &GetU_Velocity() const { return this->_u; }

		// Return rhoV of problem
		inline const std::vector<double> &GetV_Velocity() const { return this->_v; }

		// Return rhoW of problem
		inline const std::vector<double> &GetW_Velocity() const { return this->_w; }

		// Return rhoE of problem
		inline const std::vector<double> &GetP() const { return this->_p; }

		// Return rhoE of problem
		inline const std::vector<double> &GetE() const { return this->_E; }

		// Return rhoE of problem
		inline const std::vector<double> &GetH() const { return this->_H; }

		// Return rhoE of problem
		inline const std::vector<double> &GetFx() const { return this->_Fx; }

		// Return rhoE of problem
		inline const std::vector<double> &GetFy() const { return this->_Fy; }

		// Return rhoE of problem
		inline const std::vector<double> &GetFz() const { return this->_Fz; }

		// Return rhoE of problem
		inline const std::vector<double> &GetMach() const { return this->_M; }

		static constexpr const double E3D_PI = 3.141592653589793238;

	private:
		const E3D::Parser::SimConfig &_simConfig;

		std::vector<double> _rho;
		std::vector<double> _u;
		std::vector<double> _v;
		std::vector<double> _w;
		std::vector<double> _E;
		std::vector<double> _H;
		std::vector<double> _p;
		std::vector<double> _Fx;
		std::vector<double> _Fy;
		std::vector<double> _Fz;
		std::vector<double> _M;

		double rho_ref = 0.0;
		const double p_inf = 1.0;
		const double rho_inf = 1.0;
		double p_ref = 0.0;
		double T_ref = 0.0;
		double u_inf = 0.0;
		double v_inf = 0.0;
		double w_inf = 0.0;
		double E_inf = 0.0;
		double H_inf = 0.0;
		double M_inf = 0.0;
		double gamma_ref = 0.0;


		void Initialize(const int);
	};
}// namespace E3D::Solver