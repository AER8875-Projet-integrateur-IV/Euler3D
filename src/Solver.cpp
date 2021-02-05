#include <iostream>
#include "parser/SimConfig.hpp"
int main(int argc, char* argv[]){

    std::cout << "\n\nEuler 3D Solver." << std::endl;

    if (argc != 2) {
        std::cerr << "Usage : E3D_Solver <configFile.e3d> " << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string configFile = argv[1];
    E3D::Parser::SimConfig config(configFile);
    return 0;
}