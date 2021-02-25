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
	_rho.reserve(totalElemCount);
	_p.reserve(totalElemCount);
	_u.reserve(totalElemCount);
	_v.reserve(totalElemCount);
	_w.reserve(totalElemCount);
	_M.reserve(totalElemCount);
	_E.reserve(totalElemCount);
	_H.reserve(totalElemCount);

	//TODO RESERVE FOR FORCE VECTORS

	//

	// Initialize density vector
	std::fill(_rho.begin(), _rho.end(), rho_inf);

	// Initialize pressure vector
	std::fill(_p.begin(), _p.end(), p_inf);

	// Initialize Velocity
	std::fill(_u.begin(), _u.end(), u_inf);
	std::fill(_v.begin(), _v.end(), v_inf);
	std::fill(_w.begin(), _w.end(), w_inf);

	// Initialize Mach
	std::fill(_M.begin(), _M.end(), M_inf);

	// Initialize Energy and entropy

	std::fill(_E.begin(), _E.end(), E_inf);
	std::fill(_H.begin(), _H.end(), H_inf);

	// Initialize Forces
	std::fill(_Fx.begin(), _Fx.end(), 0.0);
	std::fill(_Fy.begin(), _Fy.end(), 0.0);
	std::fill(_Fz.begin(), _Fz.end(), 0.0);
}