#pragma once

#include "mesh/Mesh.hpp"
#include "mesh/Metrics.hpp"
#include "utils/Vector3.h"
#include <vector>

namespace E3D::Solver {

	/**
     * @brief Handle calculations regarding the aerodynamic coefficients
     * 
     * @details A lot of calculations regarding the aerodynamics coefficients can
     *      be done only once. This class will take care of doing these calculations
     *      and avoiding clutter and inefficiencies in the solvers.
     * 
     */
	class AeroCoefficients {
	private:
		std::vector<double> _cp;
		E3D::Vector3<double> _ForceCoeff;
		E3D::Vector3<double> _MomentCoeff;
		E3D::Vector3<double> _force();
		std::vector<E3D::Vector3<double>> _FaceVectors;
		std::vector<int> _BorderCells;
		int _nElem;

		// Factor in the cp equation = 1/(0.5*rho_inf*square(v_inf))
		double _cpFactor;
		double _pInf;

		void preProcessing(double pInf,
		                   double rhoInf,
		                   const E3D::Vector3<double> &uInf);

	public:
		AeroCoefficients(double pInf,
		                 double rhoInf,
		                 const E3D::Vector3<double> &uInf,
		                 const std::vector<E3D::Vector3<double>> &faceVectors);

		AeroCoefficients(double pInf,
		                 double rhoInf,
		                 const E3D::Vector3<double> &uInf,
		                 const E3D::Mesh<E3D::Parser::MeshPartition> &mesh,
		                 const E3D::Metrics &metrics);

		AeroCoefficients() = default;

		E3D::Vector3<double> &SolveCoefficients(const std::vector<double> &pressure);
	};

}// namespace E3D::Solver