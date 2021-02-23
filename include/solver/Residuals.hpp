#pragma once

#include <vector>

namespace E3D::Residuals{
    class Residuals{

        public:
            Residuals();
            void ComputeResiduals(int elem);
            void ResetResiduals(int elem);
            void UpdateResiduals(int elem);

            // Get the value of rhoV_res
            inline double GetrhoV_res() const {return _rhoV_res; }

            // Get the value of rhouV_res
            inline double GetrhouV_res() const {return _rhouV_res; }

            // Get the value of rhovV_res
            inline double GetrhovV_res() const {return _rhovV_res; }

            // Get the value of rhowV_res
            inline double GetrhowV_res() const {return _rhowV_res; }

            // Get the value of rhoHV_res
            inline double GetrhoHV_res() const {return _rhoHV_res; }

        private:
            double _rhoV_res;
            double _rhouV_res;
            double _rhovV_res;
            double _rhowV_res;
            double _rhoHV_res;
    };
}