#pragma once
#include "solver/FlowField.hpp"
#include "parser/SimConfig.hpp"
#include <cmath>
#include <vector>

namespace E3D::Solver {
    
    int FindGhostsCells(int elemID, int nbFace);    // Looping over the faces of the element and finding the ghosts cells

    // Whole process of updating BC
    void UpdateProcess(int elemID);

    // Farfield Supersonic Inflow
    std::vector<double> FarfieldSupersonicInflow(E3D::Solver::W& Winf);

    // Farfield Supersonic Outflow
    std::vector<double> FarfieldSupersonicOutflow(E3D::Solver::W& W_elem);

    // Farfield Subsonic Inflow
    void FarfieldSubsonicInflow(E3D::Parser::SimConfig &sim);

    // Farfield Subsonic Outflow
    void FarfieldSubsonicOutflow(E3D::Parser::SimConfig &sim);

    // Wall
    void Wall(E3D::Parser::SimConfig &sim);

    // Number of ghost cells around one element
    FlowField Fc(E3D::Parser::SimConfig &sim);
}