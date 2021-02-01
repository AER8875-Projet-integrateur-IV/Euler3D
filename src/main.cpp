#include <iostream>
// Quels sont les includes nécessaires?
#include "parser/SU2MeshParser.hpp"
#include "mesh/Mesh.hpp"
// #include "partition/Partition.hpp"
#include "post/post.hpp"

/**
 * @brief Entry point for the Euler3D software
 * @param argc : Number of command line parameters
 * @param argv : command line parameters in list of characters
 * @return State of the program: 0 -> all good otherwise an error is thrown (1,2,3....)
*/

int main(int argc, char *argv[]) {

    std::cout << "\n\nEuler 3D software." << std::endl;

    if (argc != 2) {
        std::cerr << "Usage : EES2D_APP <meshFileName.su2> " << std::endl;
        // exit(EXIT_FAILURE);
    }

    std::string fileName = "/home/vincentbar/Bureau/naca0012_euler_9x9x5_O_1B.su2";
    E3D::Parser::SU2MeshParser su2_parser(fileName);
    int nPart = 1;
    std::string varFile = "";
    E3D::Post::Post P(fileName, varFile, nPart);
    P.Write();
    
    return 0;
}
