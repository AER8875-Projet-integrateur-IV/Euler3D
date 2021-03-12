#pragma once

#include <vector>

namespace E3D::Solver {
	class ResidualVar {

		// ResidualVar Vector
	public:
		ResidualVar()
		    : m_rhoV_residual(0), m_rho_uV_residual(0), m_rho_vV_residual(0), m_rho_wV_residual(0), m_rho_HV_residual(0){};
		ResidualVar(double rhoV_residual, double rho_uV_residual, double rho_vV_residual, double rho_wV_residual, double rho_HV_residual)
		    : m_rhoV_residual(rhoV_residual), m_rho_uV_residual(rho_uV_residual), m_rho_vV_residual(rho_vV_residual), m_rho_wV_residual(rho_wV_residual), m_rho_HV_residual(rho_HV_residual){};


		inline ResidualVar &operator+=(ResidualVar v) {
			this->m_rhoV_residual += v.m_rhoV_residual;
			this->m_rho_uV_residual += v.m_rho_uV_residual;
			this->m_rho_vV_residual += v.m_rho_vV_residual;
			this->m_rho_wV_residual += v.m_rho_wV_residual;
			this->m_rho_HV_residual += v.m_rho_HV_residual;
			return *this;
		}


		inline ResidualVar &operator-=(ResidualVar v) {
			this->m_rhoV_residual -= v.m_rhoV_residual;
			this->m_rho_uV_residual -= v.m_rho_uV_residual;
			this->m_rho_vV_residual -= v.m_rho_vV_residual;
			this->m_rho_wV_residual -= v.m_rho_wV_residual;
			this->m_rho_HV_residual -= v.m_rho_HV_residual;
			return *this;
		}


		inline ResidualVar operator*(double &d) {
			return ResidualVar(m_rhoV_residual * d,
			                   m_rho_uV_residual * d,
			                   m_rho_vV_residual * d,
			                   m_rho_wV_residual * d,
			                   m_rho_HV_residual * d);
		}

		inline void reset() {
			this->m_rhoV_residual = 0.0;
			this->m_rho_uV_residual = 0.0;
			this->m_rho_vV_residual = 0.0;
			this->m_rho_wV_residual = 0.0;
			this->m_rho_HV_residual = 0.0;
		}


		double m_rhoV_residual;
		double m_rho_uV_residual;
		double m_rho_vV_residual;
		double m_rho_wV_residual;
		double m_rho_HV_residual;
	};

}// namespace E3D::Solver