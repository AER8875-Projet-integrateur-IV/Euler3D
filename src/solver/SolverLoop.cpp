#include "solver/SolverLoop.hpp"

using namespace E3D::Solver;

SolverLoop::SolverLoop(FlowField &FieldResults){

    std::cout << "\n"
            << "Starting solving process"
            << "\n";

    // Initialize W array
    int nElemTotal = 10;
    std::vector<W> m_W;
    m_W.reserve(nElemTotal);

}

// Update boundary conditions at every iterations.
void SolverLoop::BC_Update(){

    int nElemBC = 0;   // Identify the number of ghost cells in the partition


    for (int i = 0; i < nElemBC; i++)
    {
        /* If condition to know if the ghost cell is part of another partition or not */
        if (nElemBC = 0)
        {
            /* Insert multiple "if" "else" condition to know what is the BC */
            E3D::Solver::W W_BC = BC::FarfieldSupersonicInflow(Winf);
            // std::cout << W_BC[0] << "\n";
        }
        else if (nElemBC != 0)
        {
            /* code */
        }


    }
}


// Run the solver
void SolverLoop::Run(){

    // Update boundary conditions
	SolverLoop::BC_Update();
}

