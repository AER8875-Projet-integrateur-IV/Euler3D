#include "solver/UpdateBC.hpp"

using namespace E3D::Solver;

    
    // Function to find the number of ghost cells around on element
        // Needs the element's ID 
int FindGhostCells(int elemID, int nbFace){
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

void FarfieldSupersonicInflow(E3D::Parser::SimConfig &sim, E3D::Solver::W &Winf){
    E3D::Solver::W WGhostCells = Winf;

    double E = sim.getPressure()/((sim.getGamma()-1)*sim.getDensity())+((sim.getVelocity()*cos(sim.getAoA())*sim.getVelocity()*cos(sim.getAoA())+sim.getVelocity()*sin(sim.getAoA())*sim.getVelocity()*sin(sim.getAoA()))/2);
    std::cout << E << "\n";
    
}