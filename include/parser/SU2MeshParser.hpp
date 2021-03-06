//TODO : include licence
#pragma once


#include <fstream>
#include <string>
#include <vector>

#include "parser/Element.hpp"


namespace E3D::Parser {

	/**
     * @brief BC_Structure holds Boundary element in the following manner : [ ('fairfield',[Elem1,Elem2...]) , ('Wall',[Elem1,Elem2...]) ... ]
     * Every pair in the vector contains the BC tag and a vector of elements under this tag
     */
	using BC_Structure = std::vector<std::pair<std::string, std::vector<Element>>>;


	/**
     * @author Amin
     * @brief SU2MeshParser class Parses info from SU2 Mesh files.
     */

	class SU2MeshParser {

	public:
		/**
         * @brief Default ctor, Executes private member functions to parse data from mesh file.
         */
		SU2MeshParser(const std::string &fileName);

		/**
         * @brief prints class statistics
         */
		void printInfo();

		// Getters

		/**
         *
         * @return Number of dimensions of the geometry (2D or 3D)
         */
		inline int GetnDim() const { return this->_nDim; }

		/**
         *
         * @return total number of element (Surface + volume)
         */
		inline int GetTotalElemCount() const { return this->_nElem; }

		/**
         *
         * @return Number of Surface (or Boundary) elements
         */
		inline int GetBoundaryElemCount() const { return this->_nBoundaryElem; }

		/**
         *
         * @return NUumber of Volume Elements
         */
		inline int GetVolumeElemCount() const { return this->_nVolumeElem; }

		/**
         *
         * @return Total number of Nodes (Boundary + interior)
         */
		inline int GetPointsCount() const { return this->_nPoints; }

		/**
         *
         * @return Number of boundary conditions tags
         */
		inline int GetMarkersCount() const { return this->_nMarkers; }

		/**
         *
         * @return Number of dimensions
         */
		inline int GetNDim() const { return this->_nDim; }

		/**
         *
         * @return Vector of element object, for volume elements
         */

		inline const std::vector<Element> &GetVolumeElems() const { return this->_InteriorElements; }

		/**
         *
         * @return Pairs of BC tags and number of elements associated to the tag. Ex : [("fairfield", 64), ("wall", 128)]
         */

		inline const std::vector<std::pair<std::string, int>> &GetTags() const { return this->_tags; }

		/**
         *
         * @return Vector holding node object, holding all the nodes of the mesh
         */

		inline const std::vector<Node> &GetPoints() const { return this->_Points; }

		/**
         *
         * @return Pairs of BC tag and elements under this tag : [ ('fairfield',[Elem1,Elem2...]) , ('Wall',[Elem1,Elem2...]) ... ]
         */


		/**
         * @return Vector holding Number of faces around interior elements
         */
		inline const std::vector<int> &GetInteriorElementsFaceCount() const { return this->_InteriorElementsFaceCount; }

		inline const BC_Structure &GetBoundaryElems() const { return this->_BoundaryElements; }


		/**
         *
         * @return Vector holding Vtk ID of interior elements
         */
		inline const std::vector<int> &GetInteriorElementVtkID() const { return this->_InteriorElementsVtkID; }

	protected:
		std::string _filename;                          /** @brief SU2 mesh file path and name */
		std::ifstream _ifilestream;                     /** @brief Input file stream */
		int _nDim = 0;                                  /** @brief Geometry's number of dimensions */
		int _nElem = 0;                                 /** @brief Total number of Elements */
		int _nBoundaryElem = 0;                         /** @brief Number of surface elements */
		int _nVolumeElem = 0;                           /** @brief Number of volume elements */
		int _nPoints = 0;                               /** @brief Number of points */
		int _nMarkers = 0;                              /** @brief Number of boundary conditions */
		std::vector<Element> _InteriorElements;         /** @brief vector holding Volume elements */
		std::vector<int> _InteriorElementsVtkID;        /** @brief VTK ID of interior Elements */
		std::vector<int> _InteriorElementsFaceCount;    /** @Bbrief Number of face of interior elements */
		std::vector<Node> _Points;                      /** @brief Vector to hold all nodes of the mesh */
		std::vector<std::pair<std::string, int>> _tags; /** @brief Mesh boundaries tags and number of element (example : ("farfield",64) */
		BC_Structure _BoundaryElements;                 /** @brief Vector holding Boundary elements */


		// Volume VTK codes
		const std::pair<int, int> _VtkTetra = std::make_pair(10, 4);   /** @brief VTK code for a tetrahedron (4 Nodes) */
		const std::pair<int, int> _VtkVoxel = std::make_pair(11, 8);   /** @brief VTK code for a voxel (perfect cube) */
		const std::pair<int, int> _VtkHexa = std::make_pair(12, 8);    /** @brief VTK code for a hexahedron (8 nodes) */
		const std::pair<int, int> _VtkWedge = std::make_pair(13, 6);   /** @brief VTK code for a wedge (6 nodes) */
		const std::pair<int, int> _VtkPyramid = std::make_pair(14, 5); /** @brief VTK code for a pyramid (5 nodes) */

		// Surface VTK codes
		const std::pair<int, int> _VtkTria = std::make_pair(5, 3); /** @brief VTK code for a triangle (3 nodes) */

		const std::pair<int, int> _VtkPixel = std::make_pair(8, 4); /** @brief VTK code for a pixel (perfect rectangle) */
		const std::pair<int, int> _VtkQuad = std::make_pair(9, 4);  /** @brief VTK code for a quad (4 nodes) */


		/** @brief ordered from most to least frequently used vtkVOlumeELement to accelerate comparaison match */
		const std::vector<std::pair<int, int>> _vtkVolumeElements{_VtkTetra,
		                                                          _VtkHexa,
		                                                          _VtkPyramid,
		                                                          _VtkVoxel,
		                                                          _VtkWedge};

		/** @brief ordered from most to least frequently used vtkSurfaceELement to accelerate comparaison match */
		const std::vector<std::pair<int, int>> _vtkSurfaceElements{_VtkTria, _VtkQuad, _VtkPixel};

		/**
         * @brief Parse and write number of dimensions to a private member variable (_nDim).
         * @param filestream input file stream handle.
         */
		void parseDim();

		/**
         * @brief Parse and write number of volume elements to private member variables (_nVolumeElem).
         * @param filestream input file stream handle.
         */
		void parseVolumeElem();

		/**
         * @brief Parse and write number of points to private member variables (_nPoints).
         * @param filestream input file stream handle.
         */
		void parsePoints();

		/**
         * @brief Parse boundary condition info . Write to _nBoundaryElem and _nMarkers.
         * @param filestream input file stream handle.
         */
		void parseBC();
	};
}// namespace E3D::Parser
