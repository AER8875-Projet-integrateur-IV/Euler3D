
#include <iostream>
#include <stdlib.h>
#include <sstream>


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

    parseDim(_ifilestream);
}


void SU2MeshParser::parseDim(std::ifstream &filestream) {

    std::string line;
    bool ndim_found = false;

    while (std::getline(_ifilestream, line)) {

        if (line.find("NDIME") != std::string::npos) {

            std::stringstream ss(line);

            ss.seekg(6) >> _nDim;


            ndim_found = true;
            break;
        }

    }

    if (!ndim_found) {
        std::cerr << "Number of Dimension not found! " << std::endl;
        exit(EXIT_FAILURE);
    }

    if (_nDim != 3) {
        std::cerr << "Geometry is not 3D (NDIME != 3 )" << std::endl;
        exit(EXIT_FAILURE);
    }

}
