#pragma once

#include <vector>
#include "solver/Problem.hpp"

namespace E3D::Solver{
  class SolverLoop{
  public:

    SolverLoop();

    inline const std::vector<double> GetdeltaW() const {return this->_deltaW; }
    inline const std::vector<double> GetdeltaT() const {return this->_deltaT; }
    inline const std::vector<double> GetlocalRes() const {return this->_localRes; }
    inline const std::vector<double> GetconsFlux() const {return this->_consFlux; }

    void Run();

  private:
    std::vector<double> _deltaW;
    std::vector<double> _deltaT;
    std::vector<double> _localRes;
    std::vector<double> _consFlux;
    E3D::Solver::Problem flowfield;

    // Update boundary conditions around mesh
    void UpdateBC();

    // Calculate variables errors using "root mean square" method
    void RMS();

    // Compute "deltaW" using scheme of choice
    void DeltaW();

    // Update "W" using "deltaW"
    void UpdateW(std::vector<double> deltaW, Problem &);
  };
}