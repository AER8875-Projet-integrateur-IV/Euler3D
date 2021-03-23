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

	if (argc != 2) {
		std::cerr << "Usage : E3D_POST <configFile.e3d> " << std::endl;
		// exit(EXIT_FAILURE);
	}

	// Parsing Config file
	std::string configFile = argv[1];
	E3D::Parser::SimConfig config(configFile);

	spdlog::stopwatch globalsw;
	E3D::Logger logger(config.getPostLog());

	auto logObject = E3D::Logger::Getspdlog();
	logObject->info("Euler 3D Post-processor.\n");

	// Writing the Tecplot file
	E3D::Post::Post post = E3D::Post::Post(config);
	post.Write();

	logObject->debug("Total run time {}", globalsw);

	return 0;
}
