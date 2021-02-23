#pragma once

#include <vector>
#include "solver/FlowField.hpp"

namespace E3D::Solver{
    class RoeScheme{

        public:
            void ComputeRoe();

        private:
            void FindNormal();
            void RoeAvgs(int elem1, int elem2, E3D::Solver::FlowField& simulation);
            void RoeDelta(int elem1, int elem2, E3D::Solver::FlowField& simulation);
            double DeltaHartens(double localMach);
            void HartensCondition();
            void DeltaF(E3D::Solver::FlowField& simulation);
            void SideFluxes(E3D::Solver::FlowField& simulation);

            double _rhoTilde;
            double _uTilde;
            double _vTilde;
            double _wTilde;
            double _HTilde;
            double _cTilde;
            double _VTilde;
            double _qSquareTilde;
            double _rhoDelta;
            double _pDelta;
            double _uDelta;
            double _vDelta;
            double _wDelta;
            double _VDelta;
            std::vector<double> _deltaF1;
            std::vector<double> _deltaF234;
            std::vector<double> _deltaF5;
            double _deltaHartens;
            double _F1Hartens;
            double _F5Hartens;
            double _rightFlux;
            double _leftFlux;
            
    };
}