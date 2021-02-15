#include <iostream>
#include <memory>
#include "mesh/Mesh.hpp"
#include "utils/Logger.hpp"
#include "spdlog/logger.h"
#include "spdlog/stopwatch.h"
/**
 * @brief Entry point for the Euler3D software
 * @param argc : Number of command line parameters
 * @param argv : command line parameters in list of characters
 * @return State of the program: 0 -> all good otherwise an error is thrown (1,2,3....)
*/

int main(int argc, char *argv[]) {

    std::cout << "\n\nEuler 3D Pre-processor." << std::endl;
    
    spdlog::stopwatch globalsw;  
    E3D::Logger logger("log.txt");
    auto logObject = E3D::Logger::Getspdlog();

    if (argc != 2) {
        std::cerr << "Usage : EES2D_APP <meshFileName.su2> " << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string fileName = argv[1];
    spdlog::stopwatch meshsw;  
    E3D::Mesh mesh(fileName);
    logObject->debug("Mesh parser run time {}", meshsw);
    logObject->debug("Total run time {}", globalsw);
    return 0;
}