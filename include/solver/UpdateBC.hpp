#pragma once

namespace E3D::Solver {
    
    int FindGhostsCells(int elemID, int nbFace);    // Looping over the faces of the element and finding the ghosts cells

    // Number of ghost cells around one element
    int nbGhostCells = 0;
}