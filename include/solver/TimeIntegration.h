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
	// Method 2 to calculate the time step, found in Blazek's CFD manual ( p 176)
	double TimeStepMethod2(E3D::Solver::FlowField &_localFlowField,
                  const E3D::Mesh<E3D::Parser::MeshPartition> &_localMesh,
                  const E3D::Metrics &_localMetrics,
                  const int iElem);

    // Method 1 to calculate the time step, found in Blazek's CFD manual ( p 176)
    double TimeStepMethod1(E3D::Solver::FlowField &_localFlowField,
                                        const E3D::Mesh<E3D::Parser::MeshPartition> &_localMesh,
                                        const E3D::Metrics &_localMetrics,
                                        const int iElem);


	E3D::Solver::ConservativeVar ExplicitEuler(E3D::Solver::ResidualVar residual,
                                  double dt,
                                  const E3D::Metrics &_localMetrics,
                                  const int iElem);

}
