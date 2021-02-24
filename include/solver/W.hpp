#pragma once

#include "solver/Residuals.hpp"

namespace E3D
{
  struct W {
    W() : rho(0), rhoU(0), rhoV(0), rhoW(0), rhoE(0) {};

    W(double m_rho, double m_rhoU, double m_rhoV, double m_rhoW, double m_rhoE)
        : rho(m_rho), rhoU(m_rhoU), rhoV(m_rhoV), rhoW(m_rhoW), rhoE(m_rhoE) {}

    inline W operator+(W &v) {
		return W(rho + v.rho,
                rhoU + v.rhoU,
                rhoV + v.rhoV,
                rhoW + v.rhoW,
                rhoE + v.rhoE);
	}

	inline W operator+(W &&v) {
		return W(rho + v.rho,
                rhoU + v.rhoU,
                rhoV + v.rhoV,
                rhoW + v.rhoW,
                rhoE + v.rhoE);
	}

	inline W operator-(W &&v) {
		return W(rho - v.rho,
                rhoU - v.rhoU,
                rhoV - v.rhoV,
                rhoW - v.rhoW,
                rhoE - v.rhoE);
	}

	inline W operator-(W &v) {
		return W(rho - v.rho,
                rhoU - v.rhoU,
                rhoV - v.rhoV,
                rhoW - v.rhoW,
                rhoE - v.rhoE);
	}

	inline W operator*(double &d) {

		return W(rho * d, rhoU * d, rhoV * d, rhoW * d, rhoE * d);
	}

    // Might not need this one
	inline W operator-(E3D::Residuals::Residuals &v) {
		return W(rho - v.GetrhoV_res(),
                rhoU - v.GetrhouV_res(),
                rhoV - v.GetrhovV_res(),
                rhoW - v.GetrhowV_res(),
                rhoE - v.GetrhoHV_res());
		}

	inline W operator*(E3D::Residuals::Residuals &v) {
        return W(rho * v.GetrhoV_res(),
                rhoU * v.GetrhouV_res(),
                rhoV * v.GetrhovV_res(),
                rhoW * v.GetrhowV_res(),
                rhoE * v.GetrhoHV_res());
		}

	inline W &operator+=(E3D::Residuals::Residuals &&v) {
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
}