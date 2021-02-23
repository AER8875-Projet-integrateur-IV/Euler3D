#include "mesh/Mesh.hpp"
#include "partition/Partition.hpp"
#include "spdlog/logger.h"
#include "spdlog/stopwatch.h"
#include "utils/Logger.hpp"
#include <iostream>
#include <memory>
/**
 * @brief Entry point for the Euler3D software
 * @param argc : Number of command line parameters
 * @param argv : command line parameters in list of characters
 * @return State of the program: 0 -> all good otherwise an error is thrown (1,2,3....)
*/

int main(int argc, char *argv[]) {


	if (argc != 2) {
		std::cerr << "Usage : EES2D_APP <meshFileName.su2> " << std::endl;
		exit(EXIT_FAILURE);
	}

	spdlog::stopwatch globalsw;
	E3D::Logger logger("log.txt");
	auto logObject = E3D::Logger::Getspdlog();
	logObject->info("Euler 3D Pre-processor.\n");

	std::string fileName = argv[1];

	spdlog::stopwatch meshsw;
	E3D::Mesh mesh(fileName);

	mesh.solveConnectivity();
	logObject->debug("Mesh parser run time {}", meshsw);

	int nPart = 5;
	std::string SU2OuputPath = "mesh_partition_#.su2";
	E3D::Partition::Partition part(&mesh, nPart);
	part.Write(SU2OuputPath);


	logObject->debug("Total run time {}", globalsw);
	return 0;
}
