//
// Created by amin on 3/1/21.
//
#pragma once
#include <string>
namespace E3D::Solver {

	void printHeader(){
		std::cout << "Iterations" << "\n";
		std::cout << std::string(43,'-') << "\n";
	}
	void PrintSolverIteration(double CL, double CD, double RMS, double walltime, int iteration) {
		printf("            | CL         : %*.*f       |\n", 8,4,CL);
        printf(" %*d      | CD         : %*.*f       |\n", 5,iteration, 8,4,CD);
        printf("            | Rms_rho    : %*.*f       |\n", 8,4,RMS);
        printf("            | Wall Time  : %*.*f s     |\n", 8,4,walltime);
		printf("------------------------------------------\n");
	}
}