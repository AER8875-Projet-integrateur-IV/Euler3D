#include "mesh/Mesh.hpp"
#include "partition/Partition.hpp"
#include <iostream>
/**
 * @brief Entry point for the Euler3D software
 * @param argc : Number of command line parameters
 * @param argv : command line parameters in list of characters
 * @return State of the program: 0 -> all good otherwise an error is thrown (1,2,3....)
*/

int main(int argc, char *argv[]) {

	std::cout << "\n\nEuler 3D software." << std::endl;
	/* 
	if (argc != 2) {
		std::cerr << "Usage : EES2D_APP <meshFileName.su2> " << std::endl;
		exit(EXIT_FAILURE);
	}

 */
	std::string fileName = "/home/aziz/Bureau/A2020/PI4/Codes/Euler3D/test/mesh/naca0012_9x9x5_unittest_mesh.su2";

	E3D::Mesh *mesh = new E3D::Mesh(fileName);
	int nPart = 4;
	E3D::Partition::Partition part(mesh, nPart);
	part.Write();

	delete mesh;

	return 0;
}