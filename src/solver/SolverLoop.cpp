#include "solver/SolverLoop.hpp"

using namespace E3D::Solver;

SolverLoop::SolverLoop(E3D::Solver::FlowField &FieldResults){

    std::cout << "\n"
            << "Starting solving process"
            << "\n";

    // Initialize W array
    int nElemTotal = 10;
    m_W = new W[nElemTotal];   
}

// Update boundary conditions at every iterations.
void SolverLoop::UpdateBC(){

    int nElemBC = 0;   // Identify the number of ghost cells in the partition
    

    for (int i = 0; i < nElemBC; i++)
    {
        /* If condition to know if the ghost cell is part of another partition or not */
        if (nElemBC = 0)
        {
            /* Insert multiple "if" "else" condition to know what is the BC */
        }
        else if (nElemBC != 0)
        {
            /* code */
        }
        
        
    }
    

}