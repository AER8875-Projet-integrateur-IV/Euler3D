#include "solver/SolverLoop.hpp"
#include "solver/UpdateBC.hpp"

E3D::SolverLoop::SolverLoop(FlowField &FieldResults){

    std::cout << "\n"
            << "Starting solving process"
            << "\n";

    // Initialize W array
    int nElemTotal = 10;
    std::vector<E3D::W> m_W;
    m_W.reserve(nElemTotal);

    // Initialize Winf object
    Winf.rho = 1;
    Winf.rhoE = 2;
    Winf.rhoU = 3;
    Winf.rhoV = 4;
    Winf.rhoW = 5;

}

// Update boundary conditions at every iterations.
void E3D::SolverLoop::BC_Update(){

    int nElemBC = 0;   // Identify the number of ghost cells in the partition


    for (int i = 0; i < 2; i++)
    {
        /* If condition to know if the ghost cell is part of another partition or not */
        if (nElemBC == 0)
        {
            /* Insert multiple "if" "else" condition to know what is the BC */
            E3D::W W_BC = E3D::BC::FarfieldSupersonicInflow(Winf);
        }
        else if (nElemBC != 0)
        {
            /* code */
        }


    }
}


// Run the solver
void E3D::SolverLoop::Run(){

    // Update boundary conditions
	SolverLoop::BC_Update();
}

