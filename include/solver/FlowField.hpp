#pragma once

#include "mesh/Mesh.hpp"
#include "parser/SimConfig.hpp"
#include "solver/ConservativeVar.hpp"
#include <iostream>
#include <vector>

namespace E3D::Solver {

	class FlowField {

	public:
		FlowField(const E3D::Parser::SimConfig &config,
		          const E3D::Mesh<E3D::Parser::MeshPartition> &);

		void PrintTest();

		void Update(const std::vector<E3D::Solver::ConservativeVar> &delW_vector);

		// Return rho (density) of problem

		inline std::vector<double> &Getrho() { return this->_rho; }

		// Return rhoU of problem
		inline std::vector<double> &GetU_Velocity() { return this->_u; }

		// Return rhoV of problem
		inline std::vector<double> &GetV_Velocity() { return this->_v; }

		// Return rhoW of problem
		inline std::vector<double> &GetW_Velocity() { return this->_w; }

		// Return rhoE of problem
		inline const std::vector<double> &GetP() const { return this->_p; }

		// Return rhoE of problem
		inline std::vector<double> &GetE() { return this->_E; }

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

		inline double getrho_ref() const { return rho_ref; }

		inline double getp_inf() const { return p_inf; }

		inline double getrho_inf() const { return rho_inf; }

		inline double getp_ref() const { return p_ref; }

		inline double getT_ref() const { return T_ref; }

		inline double getu_inf() const { return u_inf; }

		inline double getv_inf() const { return v_inf; }

		inline double getw_inf() const { return w_inf; }

		inline double getE_inf() const { return E_inf; }

		inline double getH_inf() const { return H_inf; }

		inline double getM_inf() const { return M_inf; }

		inline double getCFL() const { return cfl; }

		inline double getgamma_ref() const { return gamma_ref; }

		inline double getSoundspd_ref() const { return SoundSpd_ref; }

		inline int getTotalElemsCount() const { return _totalElemCount; }

		inline int getTotalDomainCounts() const { return _TotalDomainElems; }

		static constexpr const double E3D_PI = 3.141592653589793238;

		inline void setRho(int ID, double value) { _rho[ID] = value; }

		inline void setU(int ID, double value) { _u[ID] = value; }

		inline void setV(int ID, double value) { _v[ID] = value; }

		inline void setW(int ID, double value) { _w[ID] = value; }

		inline void setE(int ID, double value) { _E[ID] = value; }

		inline void setH(int ID, double value) { _H[ID] = value; }

		inline void setP(int ID, double value) { _p[ID] = value; }

		inline void setFx(int ID, double value) { _Fx[ID] = value; }

		inline void setFy(int ID, double value) { _Fy[ID] = value; }

		inline void setFz(int ID, double value) { _Fz[ID] = value; }

		inline void setM(int ID, double value) { _M[ID] = value; }

	private:
		const E3D::Parser::SimConfig &_simConfig;

		std::vector<double> _rho;
		std::vector<double> _u;
		std::vector<double> _v;
		std::vector<double> _w;
		std::vector<double> _E;
		std::vector<double> _H;
		std::vector<double> _rhou;
		std::vector<double> _rhov;
		std::vector<double> _rhow;
		std::vector<double> _rhoE;
		std::vector<double> _p;
		std::vector<double> _Fx;
		std::vector<double> _Fy;
		std::vector<double> _Fz;
		std::vector<double> _M;

		double cfl = 0.0;
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
		double SoundSpd_ref = 0.0;
		int _totalElemCount = 0;
		int _interiorElemCount = 0;
		int _TotalDomainElems = 0;


		void Initialize(const int, const int);
	};
}// namespace E3D::Solver