#pragma once
#include "solver/FlowField.hpp"
#include "parser/SimConfig.hpp"
#include <cmath>

namespace E3D::Solver {
    
    int FindGhostsCells(int elemID, int nbFace);    // Looping over the faces of the element and finding the ghosts cells

    // Whole process of updating BC
    void UpdateProcess(int elemID);

    // Farfield Supersonic Inflow
    void FarfieldSupersonicInflow(E3D::Parser::SimConfig &sim);

    // Number of ghost cells around one element
    FlowField Fc;
    int nbGhostCells = 0;
}