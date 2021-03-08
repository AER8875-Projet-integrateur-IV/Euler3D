#pragma once

#include "solver/ResidualVar.hpp"

namespace E3D::Solver {
	struct ConservativeVar {
		ConservativeVar() : rho(0), rhoU(0), rhoV(0), rhoW(0), rhoE(0){};

		ConservativeVar(double m_rho, double m_rhoU, double m_rhoV, double m_rhoW, double m_rhoE)
		    : rho(m_rho), rhoU(m_rhoU), rhoV(m_rhoV), rhoW(m_rhoW), rhoE(m_rhoE) {}

		inline ConservativeVar operator+(ConservativeVar &v) {
			return ConservativeVar(rho + v.rho,
			                       rhoU + v.rhoU,
			                       rhoV + v.rhoV,
			                       rhoW + v.rhoW,
			                       rhoE + v.rhoE);
		}

		inline ConservativeVar operator+(ConservativeVar &&v) {
			return ConservativeVar(rho + v.rho,
			                       rhoU + v.rhoU,
			                       rhoV + v.rhoV,
			                       rhoW + v.rhoW,
			                       rhoE + v.rhoE);
		}

		inline ConservativeVar operator-(ConservativeVar &&v) {
			return ConservativeVar(rho - v.rho,
			                       rhoU - v.rhoU,
			                       rhoV - v.rhoV,
			                       rhoW - v.rhoW,
			                       rhoE - v.rhoE);
		}

		inline ConservativeVar operator-(ConservativeVar &v) {
			return ConservativeVar(rho - v.rho,
			                       rhoU - v.rhoU,
			                       rhoV - v.rhoV,
			                       rhoW - v.rhoW,
			                       rhoE - v.rhoE);
		}

		inline ConservativeVar operator*(double &d) {

			return ConservativeVar(rho * d, rhoU * d, rhoV * d, rhoW * d, rhoE * d);
		}

		// Might not need this one
		inline ConservativeVar operator-(E3D::Solver::ResidualVar &v) {
			return ConservativeVar(rho - v.GetrhoV_res(),
			                       rhoU - v.GetrhouV_res(),
			                       rhoV - v.GetrhovV_res(),
			                       rhoW - v.GetrhowV_res(),
			                       rhoE - v.GetrhoHV_res());
		}

		inline ConservativeVar operator*(E3D::Solver::ResidualVar &v) {
			return ConservativeVar(rho * v.GetrhoV_res(),
			                       rhoU * v.GetrhouV_res(),
			                       rhoV * v.GetrhovV_res(),
			                       rhoW * v.GetrhowV_res(),
			                       rhoE * v.GetrhoHV_res());
		}

		inline ConservativeVar &operator+=(E3D::Solver::ResidualVar &&v) {
			this->rho += v.GetrhoV_res();
			this->rhoU += v.GetrhouV_res();
			this->rhoV += v.GetrhovV_res();
			this->rhoW += v.GetrhowV_res();
			this->rhoE += v.GetrhoHV_res();
			return *this;
		}

		double rho;
		double rhoU;
		double rhoV;
		double rhoW;
		double rhoE;
	};
}// namespace E3D::Solver