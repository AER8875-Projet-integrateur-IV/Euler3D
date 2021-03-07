#pragma once

#include "solver/ResidualVar.hpp"

namespace E3D::Solver
{
    struct ConservativeVar {
        ConservativeVar() : rho(0), rhoU(0), rhoV(0), rhoW(0), rhoE(0) {};

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
            return ConservativeVar(rho - v.m_rhoV_residual,
                     rhoU - v.m_rho_uV_residual,
                     rhoV - v.m_rho_vV_residual,
                     rhoW - v.m_rho_wV_residual,
                     rhoE - v.m_rho_HV_residual);
        }

        inline ConservativeVar operator*(E3D::Solver::ResidualVar &v) {
            return ConservativeVar(rho * v.m_rhoV_residual,
                     rhoU * v.m_rho_uV_residual,
                     rhoV * v.m_rho_vV_residual,
                     rhoW * v.m_rho_wV_residual,
                     rhoE * v.m_rho_HV_residual);
        }

        inline ConservativeVar &operator+=(E3D::Solver::ResidualVar &&v) {
            this->rho += v.m_rhoV_residual;
            this->rhoU += v.m_rho_uV_residual;
            this->rhoV += v.m_rho_vV_residual;
            this->rhoW += v.m_rho_wV_residual;
            this->rhoE += v.m_rho_HV_residual;
            return *this;
        }

        double rho;
        double rhoU;
        double rhoV;
        double rhoW;
        double rhoE;


    };
}