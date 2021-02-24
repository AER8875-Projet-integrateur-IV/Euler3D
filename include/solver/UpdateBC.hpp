#pragma once
#include "solver/FlowField.hpp"
#include "parser/SimConfig.hpp"
#include <cmath>
#include <vector>

namespace E3D::BC {
    
    int FindGhostsCells(int elemID, int nbFace);    // Looping over the faces of the element and finding the ghosts cells

    // Whole process of updating BC
    void UpdateProcess(int elemID);

    // Farfield Supersonic Inflow
    E3D::W FarfieldSupersonicInflow(E3D::W& Winf);

    // Farfield Supersonic Outflow
    std::vector<double> FarfieldSupersonicOutflow(E3D::W& W_elem);

    // Farfield Subsonic Inflow
    std::vector<double> FarfieldSubsonicInflow(E3D::Parser::SimConfig& sim, E3D::W& W_elem, E3D::W& Winf);    // Will also need to include the metrics

    // Farfield Subsonic Outflow
    void FarfieldSubsonicOutflow(E3D::Parser::SimConfig &sim);

    // Wall
    void Wall(E3D::Parser::SimConfig &sim);

    // Number of ghost cells around one element
    FlowField Fc(E3D::Parser::SimConfig &sim);


}