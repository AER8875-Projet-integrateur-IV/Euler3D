#pragma once


#include <fstream>
#include <string>
#include <vector>


namespace E3D::Parser {

	/**
     * @author Aziz et Vincentt
     * @brief Parser for solution
     */

	class BoundaryPost {

	public:
		/**
         * @brief Default ctor, Executes private member functions to parse data from mesh file.
         */
		BoundaryPost(std::string fileName);
		~BoundaryPost();

		// Getters

		/**
         *
         * @return NUumber of Volume Elements
         */
		inline int GetAdjacentCell(int iGhost) const { return this->_adjacentElement[iGhost]; }
		inline const int *GetWallNodes(int iGhost, int &nNode) const {
			int starti = this->_nodeStart[iGhost];
			int endi = this->_nodeStart[iGhost + 1];
			nNode = endi - starti;
			return this->_node.data() + starti;
		}
		inline int GetWallElementCount() const { return this->_nWallCells; };

	private:
		std::string _filename;      /** @brief SU2 mesh file path and name */
		std::ifstream _ifilestream; /** @brief Input file stream */
		int _nWallCells;
		std::vector<int> _adjacentElement;
		std::vector<int> _nodeStart;
		std::vector<int> _node;

		void parseFile();
	};
}// namespace E3D::Parser
