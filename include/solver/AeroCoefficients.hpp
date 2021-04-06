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
		E3D::Vector3<double> _Force;
		E3D::Vector3<double> _CentrePressure;
		E3D::Vector3<double> _ReferencePoint;
		E3D::Vector3<double> _ForceCoeff;
		E3D::Vector3<double> _MomentCoeff;
		E3D::Vector3<double> _SumFaceVector;

		std::vector<E3D::Vector3<double>> _FaceVectors;
		std::vector<E3D::Vector3<double>> _FaceCentroids;

		std::vector<int> _BorderCells;
		int _nElem;

		// Factor in the cp equation = 1/(0.5*rho_inf*square(v_inf))
		double _cpFactor;
		double _pInf;
		double _Sref;
		double _Cref;

	public:
		AeroCoefficients(double pInf,
		                 double rhoInf,
		                 const E3D::Vector3<double> &uInf,
		                 const E3D::Mesh<E3D::Parser::MeshPartition> &mesh,
		                 const E3D::Metrics &metrics,
		                 const E3D::Vector3<double> &refPoint,
		                 double Sref,
		                 double Cref);
		AeroCoefficients() = default;
		void Update(const std::vector<double> &pressure);
		inline const E3D::Vector3<double> &GetForceCoeff() const { return _ForceCoeff; };
		inline const E3D::Vector3<double> &GetMomentCoeff() const { return _MomentCoeff; };
		inline const std::vector<double> &GetCp() const { return _cp; };
		inline const E3D::Vector3<double> &GetSumFaceVectors() const { return _SumFaceVector; };
		inline const std::vector<E3D::Vector3<double>> &GetCentroids() const { return _FaceCentroids; };
	};

}// namespace E3D::Solver