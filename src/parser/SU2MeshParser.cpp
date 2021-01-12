
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <iomanip>

#include "parser/SU2MeshParser.hpp"

using E3D::SU2MeshParser;

SU2MeshParser::SU2MeshParser(const std::string &filename)
        : _filename(filename), _ifilestream(filename) {

    std::cout << "SU2MeshParser class initialized !" << std::endl;

    //File operation check
    if (!_ifilestream) {
        std::cerr << "Error while opening mesh file ! " << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << std::setw(40)
              << "Mesh File Name : "
              << std::setw(6)
              << _filename
              << "\n";

    parseDim(_ifilestream);
    parseVolumeElem(_ifilestream);
    parsePoints(_ifilestream);
}


void SU2MeshParser::parseDim(std::ifstream &filestream) {

    std::string line;
    bool ndim_found = false;

    while (std::getline(_ifilestream, line)) {

        if (line.find("NDIME=") != std::string::npos) {

            std::stringstream ss(line);

            ss.seekg(6) >> _nDim;


            ndim_found = true;
            break;
        }

    }

    if (!ndim_found) {
        std::cerr << "Dimension keyword not found \"NDIME=\" ! " << std::endl;
        exit(EXIT_FAILURE);
    }

    if (_nDim != 3) {
        std::cerr << "Geometry is not 3D (NDIME != 3 )" << std::endl;
        exit(EXIT_FAILURE);
    }


}

void SU2MeshParser::parseVolumeElem(std::ifstream &) {
    std::string line;
    bool nelem_found = false;

    while (std::getline(_ifilestream, line)) {

        if (line.find("NELEM=") != std::string::npos) {

            std::stringstream ss(line);

            ss.seekg(6) >> _nVolumeElem;

            nelem_found = true;
            break;
        }

    }

    if (!nelem_found) {
        std::cerr << "Dimension keyword not found \"NELEM=\" ! " << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << std::setw(40)
              << "Number of Volume Elements : "
              << std::setw(6)
              << _nVolumeElem
              << "\n";
}

void SU2MeshParser::parsePoints(std::ifstream &) {
    std::string line;
    bool npoint_found = false;

    while (std::getline(_ifilestream, line)) {

        if (line.find("NPOIN=") != std::string::npos) {

            std::stringstream ss(line);

            ss.seekg(6) >> _nPoints;

            npoint_found = true;
            break;
        }

    }

    if (!npoint_found) {
        std::cerr << "Dimension keyword not found \"NPOIN=\" ! " << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << std::setw(40)
              << "Number of Nodes : "
              << std::setw(6)
              << _nPoints
              << "\n";
}