#pragma once

#include <iostream>

namespace E3D::Solver
{
  class Problem {

  public:

    Problem();

    void PrintTest();

    // Return rho (density) of problem
    inline double Getrho() const {return this->_rho; }

    // Return rhoU of problem
    inline double GetrhoU() const {return this->_rhoU; }

    // Return rhoV of problem
    inline double GetrhoV() const {return this->_rhoV; }

    // Return rhoW of problem
    inline double GetrhoW() const {return this->_rhoW; }

    // Return rhoE of problem
    inline double GetrhoE() const {return this->_rhoE; }
  private:

    double _rho;
    double _rhoU;
    double _rhoV;
    double _rhoW;
    double _rhoE;
  };
}