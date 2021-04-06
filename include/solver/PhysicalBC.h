//
// Created by amin on 3/3/21.
//
#pragma once
#include "mesh/Metrics.hpp"
#include "solver/ConservativeVar.hpp"
#include "solver/FlowField.hpp"

namespace E3D::Solver::BC {
	/**
	 * @brief updates Ghost cell state with Supersonic outflow equations;
	 * @param flowfield  class FlowField by reference
	 * @param GhostcellID ID of ghost cell
	 * @param InteriorCellID Interior Volume cell adjacent to the ghost cell
	 */
	void FarfieldSupersonicOutflow(E3D::Solver::FlowField &flowfield,
	                               const int GhostcellID,
	                               const int InteriorCellID);

	/**
    * @brief updates Ghost cell state with Supersonic inflow equations;
    * @param flowfield  class FlowField by reference
    * @param GhostcellID ID of ghost cell
    */
	void FarfieldSupersonicInflow(E3D::Solver::FlowField &flowfield, const int GhostcellID);

	/**
    * @brief updates Ghost cell state with Supersonic outflow equations;
    * @param flowfield  class FlowField by reference
    * @param GhostcellID ID of ghost cell
    * @param InteriorCellID Interior Volume cell adjacent to the ghost cell
    * @param FaceID face between ghost cell and interior element
    */
	void FarfieldSubsonicOutflow(E3D::Solver::FlowField &flowfield,
	                             const E3D::Metrics &localMetrics,
	                             const int GhostcellID,
	                             const int InteriorCellID,
	                             const int FaceID);

	/**
    * @brief updates Ghost cell state with Supersonic outflow equations;
    * @param flowfield  class FlowField by reference
    * @param GhostcellID ID of ghost cell
    * @param InteriorCellID Interior Volume cell adjacent to the ghost cell
    * @param FaceID face between ghost cell and interior element
    */
	void FarfieldSubsonicInflow(E3D::Solver::FlowField &flowfield,
	                            const E3D::Metrics &localMetrics,
	                            const int GhostcellID,
	                            const int InteriorCellID,
	                            const int FaceID);

	/**
    * @brief updates Ghost cell state with wall/airfoil equations;
    * @param flowfield  class FlowField by reference
    * @param GhostcellID ID of ghost cell
    * @param InteriorCellID Interior Volume cell adjacent to the ghost cell
    * @param FaceID face between ghost cell and interior element
    */
	void Wall(E3D::Solver::FlowField &flowfield,
	          const E3D::Metrics &localMetrics,
	          const int GhostcellID,
	          const int InteriorCellID,
	          const int FaceID);


	/**
    * @brief updates Ghost cell state with Symmetry equations;
    * @param flowfield  class FlowField by reference
    * @param GhostcellID ID of ghost cell
    * @param InteriorCellID Interior Volume cell adjacent to the ghost cell
    * @param FaceID face between ghost cell and interior element
    */
	void Symmetry(E3D::Solver::FlowField &flowfield,
                  const E3D::Metrics &localMetrics,
	              const int GhostcellID,
	              const int InteriorCellID,
	              const int FaceID);


}// namespace E3D::Solver::BC
