#include "solver/FlowField.hpp"

#include <cmath>
#include <iostream>

using namespace E3D::Solver;

FlowField::FlowField(const E3D::Parser::SimConfig &config,
                     const E3D::Mesh<E3D::Parser::MeshPartition> &localMesh) : _simConfig(config) {

	if (localMesh.getMeshRankID() == 0) {

		std::cout << "\n\n"
		          << std::string(24, '#') << "  FlowField Initialization  " << std::string(24, '#') << "\n\n";
	}


	// Compute Number of cells connected airfoil or wall

	std::vector<int> WallTagIDs;
	// TOTAL number
	int nbOfTags = 0;
	for (auto &[TagName, NbElems] : localMesh.GetBoundaryConditionVector()) {

		auto Tag = TagName;
		std::transform(Tag.begin(), Tag.end(), Tag.begin(), ::tolower);
		if (Tag == "airfoil" || Tag == "wall") {
			WallTagIDs.push_back(nbOfTags);
		}
		nbOfTags++;
	};

	int NbWallElems = 0;
	for (auto &WallID : WallTagIDs) {
		NbWallElems += localMesh.GetNumberOfElementsInTag(WallID);
	}


	double startInitializationTimer = MPI_Wtime();

	int TotalNumberOfBoundaryElems = 0;
	for (int i = 0; i < nbOfTags; i++) {
		TotalNumberOfBoundaryElems += localMesh.GetNumberOfElementsInTag(i);
	}

	double totalElemCount = localMesh.GetMeshInteriorElemCount() + localMesh.GetMpiElemsCount() + localMesh.GetWallGhostCellsIDs().size() + localMesh.GetFarfieldGhostCellsIDs().size() + localMesh.GetSymmetryGhostCellsIDs().size();
	_totalElemCount = totalElemCount;
	_interiorElemCount = localMesh.GetMeshInteriorElemCount();

	Initialize(totalElemCount, NbWallElems);

	MPI_Allreduce(&_interiorElemCount, &_TotalDomainElems, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);


	MPI_Barrier(MPI_COMM_WORLD);
	if (localMesh.getMeshRankID() == 0) {
		double endInitializationTimer = MPI_Wtime();
		printf("Initializing FlowField took %.5f seconds .\n", endInitializationTimer - startInitializationTimer);
	}
}

void FlowField::PrintTest() {
	std::cout << "hello"
	          << "\n";
}


void FlowField::Initialize(const int totalElemCount, const int ForceElemsCount) {
	//TODO ADD ANGLE FOR SIDE FLOW (W_INF)

	// update reference variables
	cfl = _simConfig.getCFL();
	gamma_ref = _simConfig.getGamma();
	M_inf = _simConfig.getMach();
	u_inf = M_inf * sqrt(gamma_ref) * std::cos(_simConfig.getAoA() * (E3D_PI / 180));
	v_inf = M_inf * sqrt(gamma_ref) * std::sin(_simConfig.getAoA() * (E3D_PI / 180));
	p_ref = _simConfig.getPressure();
	T_ref = _simConfig.getTemperature();
	rho_ref = _simConfig.getDensity();
	E_inf = p_inf / ((gamma_ref - 1) * rho_inf) + ((u_inf * u_inf + v_inf * v_inf + w_inf * w_inf) / 2.0);
	H_inf = E_inf + (p_inf / rho_inf);
	SoundSpd_ref = sqrt(gamma_ref * (p_ref / rho_ref));

	// resize vectors to infinite flowfield state
	_rho.resize(totalElemCount, rho_inf);
	_p.resize(totalElemCount, p_inf);
	_u.resize(totalElemCount, u_inf);
	_rhou.resize(_interiorElemCount, rho_inf * u_inf);
	_v.resize(totalElemCount, v_inf);
	_rhov.resize(_interiorElemCount, rho_inf * v_inf);
	_w.resize(totalElemCount, w_inf);
	_rhow.resize(_interiorElemCount, rho_inf * w_inf);
	_M.resize(totalElemCount, M_inf);
	_E.resize(totalElemCount, E_inf);
	_rhoE.resize(_interiorElemCount, rho_inf * E_inf);
	_H.resize(totalElemCount, H_inf);

	_Fx.resize(ForceElemsCount, 0);
	_Fy.resize(ForceElemsCount, 0);
	_Fz.resize(ForceElemsCount, 0);
}

void FlowField::Update(const std::vector<E3D::Solver::ConservativeVar> &delW_vector, const std::vector<int> &MPIids,
                       const std::vector<int> &adjacentToMPIids) {
	// Update interior Cells
	for (size_t ielem = 0; ielem < delW_vector.size(); ielem++) {

		_rho[ielem] += delW_vector[ielem].rho;


		_rhou[ielem] += delW_vector[ielem].rhoU;

		_rhov[ielem] += delW_vector[ielem].rhoV;
		_rhow[ielem] += delW_vector[ielem].rhoW;
		_rhoE[ielem] += delW_vector[ielem].rhoE;

		_u[ielem] = _rhou[ielem] / _rho[ielem];
		_v[ielem] = _rhov[ielem] / _rho[ielem];
		_w[ielem] = _rhow[ielem] / _rho[ielem];
		_E[ielem] = _rhoE[ielem] / _rho[ielem];
		_p[ielem] = (gamma_ref - 1) * _rho[ielem] * (_E[ielem] - (std::pow(_u[ielem], 2) + std::pow(_v[ielem], 2) + std::pow(_w[ielem], 2)) / 2.0);
		_H[ielem] = _E[ielem] + (_p[ielem] / _rho[ielem]);

		_M[ielem] = sqrt(gamma_ref * (_p[ielem] / _rho[ielem])) / sqrt((std::pow(_u[ielem], 2) + std::pow(_v[ielem], 2) + std::pow(_w[ielem], 2)));
	}
	//Update Ghost Cells
	for (size_t ielem = 0; ielem < MPIids.size(); ielem++) {
		_rho[MPIids[ielem]] = _rho[adjacentToMPIids[ielem]];

		_u[MPIids[ielem]] = _u[adjacentToMPIids[ielem]];
		_v[MPIids[ielem]] = _v[adjacentToMPIids[ielem]];
		_w[MPIids[ielem]] = _w[adjacentToMPIids[ielem]];
		_E[MPIids[ielem]] = _E[adjacentToMPIids[ielem]];
		_p[MPIids[ielem]] = _p[adjacentToMPIids[ielem]];
		_H[MPIids[ielem]] = _H[adjacentToMPIids[ielem]];
		_M[MPIids[ielem]] = _M[adjacentToMPIids[ielem]];
	}
}

void FlowField::updateWRungeKutta(const std::vector<E3D::Solver::ConservativeVar> &delW_vector, const std::vector<E3D::Solver::ConservativeVar> &W0, const std::vector<int> &MPIids,
                                  const std::vector<int> &adjacentToMPIids) {

	for (size_t ielem = 0; ielem < delW_vector.size(); ielem++) {
		_rho[ielem] = W0[ielem].rho - delW_vector[ielem].rho;
		_rhou[ielem] = W0[ielem].rhoU - delW_vector[ielem].rhoU;
		_rhov[ielem] = W0[ielem].rhoV - delW_vector[ielem].rhoV;
		_rhow[ielem] = W0[ielem].rhoW - delW_vector[ielem].rhoW;
		_rhoE[ielem] = W0[ielem].rhoE - delW_vector[ielem].rhoE;

		_u[ielem] = _rhou[ielem] / _rho[ielem];
		_v[ielem] = _rhov[ielem] / _rho[ielem];
		_w[ielem] = _rhow[ielem] / _rho[ielem];
		_E[ielem] = _rhoE[ielem] / _rho[ielem];
		_p[ielem] = (gamma_ref - 1) * _rho[ielem] * (_E[ielem] - (std::pow(_u[ielem], 2) + std::pow(_v[ielem], 2) + std::pow(_w[ielem], 2)) / 2.0);
		_H[ielem] = _E[ielem] + (_p[ielem] / _rho[ielem]);

		_M[ielem] = sqrt(gamma_ref * (_p[ielem] / _rho[ielem])) / sqrt((std::pow(_u[ielem], 2) + std::pow(_v[ielem], 2) + std::pow(_w[ielem], 2)));
	}
	//Update Ghost Cells
	for (size_t ielem = 0; ielem < MPIids.size(); ielem++) {
		_rho[MPIids[ielem]] = _rho[adjacentToMPIids[ielem]];

		_u[MPIids[ielem]] = _u[adjacentToMPIids[ielem]];
		_v[MPIids[ielem]] = _v[adjacentToMPIids[ielem]];
		_w[MPIids[ielem]] = _w[adjacentToMPIids[ielem]];
		_E[MPIids[ielem]] = _E[adjacentToMPIids[ielem]];
		_p[MPIids[ielem]] = _p[adjacentToMPIids[ielem]];
		_H[MPIids[ielem]] = _H[adjacentToMPIids[ielem]];
		_M[MPIids[ielem]] = _M[adjacentToMPIids[ielem]];
	}
}