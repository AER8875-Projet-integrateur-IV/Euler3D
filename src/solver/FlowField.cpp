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


	Initialize(totalElemCount, NbWallElems);


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
	//TODO ADD ANGLE FOR SIDE FLOW (V_INF)


	// update reference variables

	gamma_ref = _simConfig.getGamma();
	M_inf = _simConfig.getMach();
	u_inf = M_inf * sqrt(gamma_ref) * std::cos(_simConfig.getAoA() * (E3D_PI / 180));
	w_inf = M_inf * sqrt(gamma_ref) * std::sin(_simConfig.getAoA() * (E3D_PI / 180));
	p_ref = _simConfig.getPressure();
	T_ref = _simConfig.getTemperature();
	rho_ref = _simConfig.getDensity();
	E_inf = p_inf / ((gamma_ref - 1) * rho_inf) + ((u_inf * u_inf + v_inf * v_inf + w_inf * w_inf) / 2);
	H_inf = E_inf + (p_inf / rho_inf);
	SoundSpd_ref = sqrt(gamma_ref * (p_ref / rho_ref));

	// resize vectors to infinite flowfield state
	_rho.resize(totalElemCount, rho_inf);
	_p.resize(totalElemCount, p_inf);
	_u.resize(totalElemCount, u_inf);
	_v.resize(totalElemCount, v_inf);
	_w.resize(totalElemCount, w_inf);
	_M.resize(totalElemCount, M_inf);
	_E.resize(totalElemCount, E_inf);
	_H.resize(totalElemCount, H_inf);

	_Fx.resize(ForceElemsCount, 0);
	_Fy.resize(ForceElemsCount, 0);
	_Fz.resize(ForceElemsCount, 0);
}