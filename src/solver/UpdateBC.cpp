#include "solver/UpdateBC.hpp"

using namespace E3D::Solver;

    
    // Function to find the number of ghost cells around on element
        // Needs the element's ID 
int FindGhostCells(int elemID, int nbFace){
    int nbGhostCells = 0;
    
    for (int i = 0; i < nbFace; i++){
        if (i=1)
        {
            nbGhostCells += 1;
        }

    } 
    return nbGhostCells;
};

void UpdateProcess(int elemID){
    // Add if condition to know if ghost cell is physical or if needs MPI communication
    
}

std::vector<double> FarfieldSupersonicInflow(E3D::Solver::W &Winf){
    std::vector<double> W_BC(5);
    W_BC[0] = Winf.rho;
    W_BC[1] = Winf.rhoU;
    W_BC[2] = Winf.rhoV;
    W_BC[3] = Winf.rhoW;
    W_BC[4] = Winf.rhoE;

    return W_BC;    
}

std::vector<double> FarfieldSupersonicOutflow(E3D::Solver::W &W_elem){
    std::vector<double> W_BC(5);
    W_BC[0] = W_elem.rho;
    W_BC[1] = W_elem.rhoU;
    W_BC[2] = W_elem.rhoV;
    W_BC[3] = W_elem.rhoW;
    W_BC[4] = W_elem.rhoE;
}

std::vector<double> FarfieldSubsonicInflow(E3D::Parser::SimConfig& sim, E3D::Solver::W& W_elem, E3D::Solver::W& Winf){
    double nx = 1;
    double ny = 1;
    double nz = 1;
}