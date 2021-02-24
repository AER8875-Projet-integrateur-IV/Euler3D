//
// Created by amin on 2/21/21.
//
#pragma once
#include "mesh/Mesh.hpp"
#include "utils/Vector3.h"
#include <parser/MeshPartition.hpp>

namespace E3D {
	class Metrics {
	public:
		/**
		 * Ctor takes Mesh class with a parser of type MeshPartition
		 */
		Metrics(const Mesh<Parser::MeshPartition>&, const Parallel::MPIHandler& e3d_mpi);

		inline const std::vector<Vector3<double>> &getFaceNormals() const { return _faceNormals; }

		inline const std::vector<Vector3<double>> &getFaceCenters() const { return _faceCenters; }

		inline const std::vector<Vector3<double>> &getCellCentroids() const { return _cellCentroids; }

		inline const std::vector<double> &getFaceSurfaces() const { return _faceSurfaces; }

		inline const std::vector<double> &getCellVolumes() const { return _cellVolumes; }

	private:
		/**
		 * @brief Compute face metrics by populating member variables _FaceNormals, _FaceCenters and _faceSurfaces
		 */
		void computeFaceMetrics();

        /**
         * @brief Compute cell metrics by populating member variables _CellVolumes and _CellCentroids
         */
		void computeCellMetrics();


		const Mesh<Parser::MeshPartition> &_localMesh;
		std::vector<Vector3<double>> _faceNormals;
		std::vector<Vector3<double>> _faceCenters;
		std::vector<Vector3<double>> _cellCentroids;
        std::vector<Vector3<double>> _faceUnitNormals;
		std::vector<double> _faceSurfaces;
		std::vector<double> _cellVolumes;

	};
}// namespace E3D
