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
			return this->_nodeLoc.data() + starti;
		}
		inline int GetWallElementCount() const { return this->_nWallCells; };
		inline int GetWallNodeCount() const { return this->_nNodes; };
		inline int GetWallGlobalNode(int localNode) const { return this->_localNode2Global[localNode]; };

	private:
		std::string _filename;      /** @brief SU2 mesh file path and name */
		std::ifstream _ifilestream; /** @brief Input file stream */
		int _nWallCells;
		std::vector<int> _adjacentElement;
		std::vector<int> _nodeStart;
		std::vector<int> _node;
		std::vector<int> _nodeLoc;
		std::vector<int> _localNode2Global;
		int _nNodes;

		void parseFile();
		void setLocal2Global();
		int saveLocalNode(int nodeG);
	};
}// namespace E3D::Parser
