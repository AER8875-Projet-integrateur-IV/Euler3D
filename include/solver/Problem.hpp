#pragma once

#include <iostream>
#include <vector>

namespace E3D::Solver
{
  class Problem {

  public:

    Problem();

    void PrintTest();

    // Return rho (density) of problem
    inline std::vector<double> Getrho() const {return this->_rho; }

    // Return rhoU of problem
    inline std::vector<double> GetrhoU() const {return this->_rhoU; }

    // Return rhoV of problem
    inline std::vector<double> GetrhoV() const {return this->_rhoV; }

    // Return rhoW of problem
    inline std::vector<double> GetrhoW() const {return this->_rhoW; }

    // Return rhoE of problem
    inline std::vector<double> GetrhoE() const {return this->_rhoE; }

  private:

    std::vector<double> _rho;
    std::vector<double> _rhoU;
    std::vector<double> _rhoV;
    std::vector<double> _rhoW;
    std::vector<double> _rhoE;
    std::vector<double> _u;
    std::vector<double> _v;
    std::vector<double> _w;
    std::vector<double> _E;
    std::vector<double> _p;
    std::vector<double> _dragCoeff;
    std::vector<double> _liftCoeff;
    std::vector<double> _pressureCoeff;
  };
}