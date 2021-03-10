//
// Created by amin on 3/7/21.
//
#pragma once

#include "mesh/Mesh.hpp"
#include "mesh/Metrics.hpp"
#include "solver/FlowField.hpp"
#include "solver/ResidualVar.hpp"

namespace E3D::Solver {
	/**
	 * @brief Discretization of Convective Fluxes using Roe upwind scheme
	 * @param iface ID of face between to elems
	 * @return Returns residual vector to be added to
	 */
	E3D::Solver::ResidualVar Roe(E3D::Solver::FlowField &,
	                             const E3D::Mesh<E3D::Parser::MeshPartition> &,
	                             const E3D::Metrics &,
	                             const int iface,
	                             bool isMPI);

	/**
     * @brief Discretization of Convective Fluxes using AUSM scheme
     * @param iface ID of face between to elems
     * @return Returns residual vector to be added to
     */
	E3D::Solver::ResidualVar AUSM(const E3D::Solver::FlowField &,
	                              const E3D::Mesh<E3D::Parser::MeshPartition> &,
	                              const E3D::Metrics &,
	                              const int iface);

	E3D::Solver::ResidualVar Fc(E3D::Solver::FlowField &localFlowField,
	                                               const E3D::Metrics &localMetrics,
	                                               const int GhostID,
	                                               const int faceID, double V);


}// namespace E3D::Solver
