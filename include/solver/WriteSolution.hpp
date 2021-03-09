#pragma once

#include "mesh/Mesh.hpp"
#include "parallelization/MPIHandler.hpp"
#include "parser/SimConfig.hpp"
#include "solver/ConservativeVar.hpp"
#include "solver/FlowField.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace E3D::Solver {

	class WriteSolution {

	public:
		WriteSolution(E3D::Solver::FlowField &flowField,
		              const E3D::Parser::SimConfig &config,
		              const E3D::Parallel::MPIHandler &e3d_mpi);
		~WriteSolution();

	private:
		E3D::Solver::FlowField & _flowField;
		int _rank;
		std::string _fileName;
		std::ofstream _file;
		void write();
	};
}// namespace E3D::Solver