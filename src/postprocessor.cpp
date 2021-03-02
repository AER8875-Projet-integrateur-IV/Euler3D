#include "mesh/Mesh.hpp"
#include "parser/SimConfig.hpp"
#include "post/post.hpp"
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

	spdlog::stopwatch globalsw;
	E3D::Logger logger("log.txt");
	auto logObject = E3D::Logger::Getspdlog();

	logObject->info("Euler 3D Post-processor.\n");
	if (argc != 2) {
		std::cerr << "Usage : E3D_POST <meshFileName.su2> " << std::endl;
		// exit(EXIT_FAILURE);
	}

	// // Parsing Config file
	// std::string configFile = argv[1];
	// E3D::Parser::SimConfig config(configFile);

	// spdlog::stopwatch globalsw;
	// // E3D::Logger logger(config.getPreLog());

	// auto logObject = E3D::Logger::Getspdlog();
	// logObject->info("Euler 3D Post-processor.\n");

	// int nPart = config.getNumberPartitions();
	// const std::vector<std::string> &SU2OuputPath = config.getPartitionedMeshFiles();
	// std::string outputFile = config.getTecplotFile();


	// int nPart = config.getNumberPartitions();
	// const std::vector<std::string> &SU2OuputPath = config.getPartitionedMeshFiles();
	// E3D::Partition::Partition part(&mesh, nPart);
	// part.Write(SU2OuputPath);

	std::vector<std::string> Path_Partition = {"/home/aziz/Bureau/A2020/PI4/Codes/Euler3D/test/post/StructuredBlock_8_p0",
	                                           "/home/aziz/Bureau/A2020/PI4/Codes/Euler3D/test/post/StructuredBlock_8_p1",
	                                           "/home/aziz/Bureau/A2020/PI4/Codes/Euler3D/test/post/StructuredBlock_8_p2",
	                                           "/home/aziz/Bureau/A2020/PI4/Codes/Euler3D/test/post/StructuredBlock_8_p3"};
	std::string outputFile = "/home/aziz/Bureau/A2020/PI4/Codes/Euler3D/test/post/Output.dat";

	E3D::Post::Post post = E3D::Post::Post(Path_Partition, outputFile);
	logObject->debug("Total run time {} s", globalsw);
	post.Write();
	return 0;
}