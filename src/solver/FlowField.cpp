#include "solver/FlowField.hpp"

#include <cmath>
#include <iostream>


using namespace E3D::Solver;

FlowField::FlowField(const E3D::Parser::SimConfig &config,
                     const E3D::Mesh<E3D::Parser::MeshPartition> &localMesh,
                     const E3D::Parallel::MPIHandler &e3d_mpi) : _simConfig(config) {
	if (e3d_mpi.getRankID() == 0) {
		std::cout << "\n\n"
		          << std::string(24, '#') << "  FlowField Initialization  " << std::string(24, '#') << "\n\n";
	}

	double startInitializationTimer = MPI_Wtime();


	double totalElemCount = localMesh.GetMeshInteriorElemCount() + localMesh.GetMpiElemsCount();
	Initialize(totalElemCount);



	MPI_Barrier(MPI_COMM_WORLD);
	if (e3d_mpi.getRankID() == 0) {
        double endInitializationTimer = MPI_Wtime();
		printf("Initializing FlowField took %.5f seconds .\n", endInitializationTimer - startInitializationTimer);
	}
}

void FlowField::PrintTest() {
	std::cout << "hello"
	          << "\n";
}

void FlowField::Initialize(const int totalElemCount) {
	//TODO ADD ANGLE FOR SIDE FLOW (V_INF)

	gamma_ref = _simConfig.getGamma();
	M_inf = _simConfig.getMach();
	u_inf = M_inf * sqrt(gamma_ref) * std::cos(_simConfig.getAoA() * (E3D_PI / 180));
	w_inf = M_inf * sqrt(gamma_ref) * std::sin(_simConfig.getAoA() * (E3D_PI / 180));
	p_ref = _simConfig.getPressure();
	T_ref = _simConfig.getTemperature();
	rho_ref = _simConfig.getDensity();

	E_inf = p_inf / ((gamma_ref - 1) * rho_inf) + ((u_inf * u_inf + v_inf * v_inf + w_inf * w_inf) / 2);
	H_inf = E_inf + (p_inf / rho_inf);

	// Reserve
	_rho.resize(totalElemCount, rho_inf);
	_p.resize(totalElemCount, p_inf);
	_u.resize(totalElemCount,u_inf);
	_v.resize(totalElemCount,v_inf);
	_w.resize(totalElemCount,w_inf);
	_M.resize(totalElemCount,M_inf);
	_E.resize(totalElemCount,E_inf);
	_H.resize(totalElemCount,H_inf);

	//TODO RESIZE AND INITIALIZATION FOR FORCE VECTORS


}