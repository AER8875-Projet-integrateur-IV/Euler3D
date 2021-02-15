//
// Created by amin on 2/5/21.
//


#include <sstream>
#include <iostream>
#include <mpi.h>
#include "parser/MeshPartition.hpp"

using namespace E3D::Parser;

MeshPartition::MeshPartition(const std::string &fileName)
        : SU2MeshParser(fileName) {
    parseMpiBoundaryElement();
    _ifilestream.close();

}

void MeshPartition::parseMpiBoundaryElement() {
    std::string line;

    bool mpi_BC_found = false;

    while (std::getline(_ifilestream, line)) {
        if (line.find("NINTERNAL_MARKER=") != std::string::npos) {

            mpi_BC_found = true;

            std::stringstream ss(line);

            ss.seekg(17) >> _nbAdjacentPartitions;
            break;
        }

    }

    if (!mpi_BC_found) {
        std::cerr << "MPI Boundary Elements keyword \"NINTERNAL_MARKER=\" not found ! " << std::endl;
        exit(EXIT_FAILURE);
    }
}

void MeshPartition::printAllPartitionsInfo(const int rankID) {
    int totalVolumeElements = 0;
    int totalSurfaceElements = 0;
    int totalNumberNodes = 0;

    MPI_Reduce(&_nVolumeElem, &totalVolumeElements, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&_nBoundaryElem, &totalSurfaceElements, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&_nPoints, &totalNumberNodes, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rankID == 0) {
        printf("\nTotal Number of Volume Elements : %d \n", totalVolumeElements);
        printf("Total Number of Surface Elements : %d \n", totalSurfaceElements);
        printf("Total Number of Nodes (including duplicates) : %d \n", totalNumberNodes);
    }
}
