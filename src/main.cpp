#include <iostream>
#include "SU2MeshParser.hpp"

/**
 * @brief Entry point for the Euler3D software
 * @param argc : Number of command line parameters
 * @param argv : command line parameters in list of characters
 * @return State of the program: 0 -> all good otherwise an error is thrown (1,2,3....)
*/

int main(int argc, char *argv[]) {

    std::cout << "Euler 3D software." << std::endl;

    E3D::SU2MeshParser su2_parser;

    return 0;
}