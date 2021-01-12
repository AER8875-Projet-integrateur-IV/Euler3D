#include <iostream>
#include "parser/SU2MeshParser.hpp"

/**
 * @brief Entry point for the Euler3D software
 * @param argc : Number of command line parameters
 * @param argv : command line parameters in list of characters
 * @return State of the program: 0 -> all good otherwise an error is thrown (1,2,3....)
*/

int main(int argc, char *argv[]) {

    std::cout << "Euler 3D software." << std::endl;

    if (argc != 2) {
        std::cerr << "Usage : EES2D_APP <meshFileName.su2> " << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string fileName = argv[1];

    E3D::SU2MeshParser su2_parser(fileName);

    return 0;
}