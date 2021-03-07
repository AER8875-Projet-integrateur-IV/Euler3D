//
// Created by amin on 3/7/21.
//

#pragma once
#include "mesh/Mesh.hpp"
#include "mesh/Metrics.hpp"
#include "solver/FlowField.hpp"
#include "solver/ConservativeVar.hpp"
#include "solver/ResidualVar.hpp"
namespace E3D::Solver{
	double TimeStep(E3D::Solver::FlowField &_localFlowField,
                  const E3D::Mesh<E3D::Parser::MeshPartition> &_localMesh,
                  const E3D::Metrics &_localMetrics,
                  const int ielem);

	E3D::Solver::ConservativeVar ExplicitEuler(E3D::Solver::ResidualVar residual,
                                  double dt,
                                  const E3D::Metrics &_localMetrics,
                                  const int iElem);

}
