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

        inline ResidualVar & operator= ( const ResidualVar & v)= default;


        inline ResidualVar &operator+=(ResidualVar&& v) {
            this->m_rhoV_residual += v.m_rhoV_residual;
            this->m_rho_uV_residual += v.m_rho_uV_residual;
            this->m_rho_vV_residual += v.m_rho_vV_residual;
            this->m_rho_wV_residual += v.m_rho_wV_residual;
            this->m_rho_HV_residual += v.m_rho_HV_residual;
            return *this;
        }

        inline ResidualVar &operator+=(ResidualVar& v) {
            this->m_rhoV_residual += v.m_rhoV_residual;
            this->m_rho_uV_residual += v.m_rho_uV_residual;
            this->m_rho_vV_residual += v.m_rho_vV_residual;
            this->m_rho_wV_residual += v.m_rho_wV_residual;
            this->m_rho_HV_residual += v.m_rho_HV_residual;
            return *this;
        }

        inline ResidualVar &operator-=(ResidualVar& v) {
            this->m_rhoV_residual -= v.m_rhoV_residual;
            this->m_rho_uV_residual -= v.m_rho_uV_residual;
            this->m_rho_vV_residual -= v.m_rho_vV_residual;
            this->m_rho_wV_residual -= v.m_rho_wV_residual;
            this->m_rho_HV_residual -= v.m_rho_HV_residual;
            return *this;
        }

        inline ResidualVar &operator-=(ResidualVar&& v) {
            this->m_rhoV_residual -= v.m_rhoV_residual;
            this->m_rho_uV_residual -= v.m_rho_uV_residual;
            this->m_rho_vV_residual -= v.m_rho_vV_residual;
            this->m_rho_wV_residual -= v.m_rho_wV_residual;
            this->m_rho_HV_residual -= v.m_rho_HV_residual;
            return *this;
        }

        inline ResidualVar operator+(ResidualVar& v) {
            return ResidualVar(this->m_rhoV_residual + v.m_rhoV_residual,
                               this->m_rho_uV_residual + v.m_rho_uV_residual,
                               this->m_rho_vV_residual + v.m_rho_vV_residual,
                               this->m_rho_wV_residual + v.m_rho_wV_residual,
                               this->m_rho_HV_residual + v.m_rho_HV_residual);
        }




        inline ResidualVar operator*(const double &d) {
            return ResidualVar(m_rhoV_residual * d,
                               m_rho_uV_residual * d,
                               m_rho_vV_residual * d,
                               m_rho_wV_residual * d,
                               m_rho_HV_residual * d);
        }

        inline ResidualVar operator/(double &d) {
            return ResidualVar(m_rhoV_residual /d,
                               m_rho_uV_residual / d,
                               m_rho_vV_residual / d,
                               m_rho_wV_residual / d,
                               m_rho_HV_residual / d);
        }

        inline ResidualVar operator/(double &&d) {
            return ResidualVar(m_rhoV_residual /d,
                               m_rho_uV_residual / d,
                               m_rho_vV_residual / d,
                               m_rho_wV_residual / d,
                               m_rho_HV_residual / d);
        }



        inline void reset() {
            this->m_rhoV_residual = 0.0;
            this->m_rho_uV_residual = 0.0;
            this->m_rho_vV_residual = 0.0;
            this->m_rho_wV_residual = 0.0;
            this->m_rho_HV_residual = 0.0;
        }

        inline double findMax() {
            double member_variables[]{std::abs(m_rhoV_residual), std::abs(m_rho_uV_residual), std::abs(m_rho_vV_residual), std::abs(m_rho_wV_residual),std::abs(m_rho_HV_residual)};
            for (auto &res : member_variables) {
                if (std::isnan(res)) {
                    std::cerr << "Error : nan residual found !" << std::endl;
                    std::exit(EXIT_FAILURE);

                }
            }
            return *std::max_element(member_variables, member_variables + 5);
        }


        double m_rhoV_residual;
        double m_rho_uV_residual;
        double m_rho_vV_residual;
        double m_rho_wV_residual;
        double m_rho_HV_residual;
    };

    inline const ResidualVar operator-(const ResidualVar v,const ResidualVar a) {
        return ResidualVar(a.m_rhoV_residual - v.m_rhoV_residual,
                           a.m_rho_uV_residual - v.m_rho_uV_residual,
                           a.m_rho_vV_residual - v.m_rho_vV_residual,
                           a.m_rho_wV_residual - v.m_rho_wV_residual,
                           a.m_rho_HV_residual - v.m_rho_HV_residual);
    }

}// namespace E3D::Solver