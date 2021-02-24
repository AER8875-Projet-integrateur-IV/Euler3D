#pragma once

#include <vector>
#include "solver/FlowField.hpp"
#include "solver/UpdateBC.hpp"
#include "parser/SimConfig.hpp"

namespace E3D {
  class SolverLoop{
  public:

    SolverLoop(E3D::FlowField& FieldResults);

    inline const std::vector<double> GetdeltaW() const {return this->_deltaW; }
    inline const std::vector<double> GetdeltaT() const {return this->_deltaT; }
    inline const std::vector<double> GetlocalRes() const {return this->_localRes; }
    inline const std::vector<double> GetconsFlux() const {return this->_convFlux; }
    inline const std::vector<double> Geterr() const {return this->_err; }

    void Run();

    // Update boundary conditions around mesh

	void BC_Update();


  private:
    std::vector<double> _deltaW;
    std::vector<double> _deltaT;
    std::vector<double> _localRes;
    std::vector<double> _convFlux;
    std::vector<double> _err;
    // E3D::Solver::FlowField flowfield;

      // Variables
      E3D::W m_W; // Array containing all the W for every element including ghost cells
      E3D::W Winf;

    // Calculate variables errors using "root mean square" method
    void RMS();

    // Compute "deltaW" using scheme of choice
    void DeltaW();

    // Update "W" using "deltaW"
    void UpdateW(std::vector<double> deltaW, FlowField &);

    // Calculate drag coefficient
    void CalcGrad();

    // Calculate lift coefficient
    void CalcCL();

    // Calculate drag coefficient
    void CalcCD();

    // Calculate pressure coefficient
    void CalcCP();


  };
}