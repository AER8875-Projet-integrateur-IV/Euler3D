#include "solver/WriteSolution.hpp"

using namespace E3D::Solver;

WriteSolution::WriteSolution(E3D::Solver::FlowField &flowField,
                             const E3D::Parser::SimConfig &config,
                             const E3D::Parallel::MPIHandler &e3d_mpi) : _flowField(flowField) {
	_fileName = config.getPartitionedMeshFiles()[e3d_mpi.getRankID()] + ".sol";
	_file = std::ofstream(_fileName);
	_file << std::setiosflags(std::ios::dec)
	      << std::setiosflags(std::ios::scientific);
	write();
	return;
}

WriteSolution::~WriteSolution() {
	_file.close();
	return;
}

void WriteSolution::write() {
	_file << std::setw(20) << "density"
	      << std::setw(20) << "velocity_x"
	      << std::setw(20) << "velocity_y"
	      << std::setw(20) << "velocity_z"
	      << std::setw(20) << "pressure"
	      << std::setw(20) << "energy"
	      << std::endl;

	std::vector<double> rho = _flowField.Getrho();
	std::vector<double> u = _flowField.GetU_Velocity();
	std::vector<double> v = _flowField.GetV_Velocity();
	std::vector<double> w = _flowField.GetW_Velocity();
	std::vector<double> p = _flowField.GetP();
	std::vector<double> E = _flowField.GetE();

	for (size_t i = 0; i < rho.size(); i++) {
		_file << std::setw(20) << rho[i]
		      << std::setw(20) << u[i]
		      << std::setw(20) << v[i]
		      << std::setw(20) << w[i]
		      << std::setw(20) << p[i]
		      << std::setw(20) << E[i]
		      << std::endl;
	}
}