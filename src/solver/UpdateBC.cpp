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