
/**
 * @file Partition.hpp
 * @author Aziz Ndiaye, Abraham Bherer-Constant
 * @date 2021/01/12
 * @brief Partition a mesh object into multiple smaller mesh objects 
 *
 * Partition a mesh on which connectivity has already been solved with
 * the METIS library into multiple smaller mesh and create a vector of
 * mesh objects with solved connectivity 
 */
#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <metis.h>

#include "mesh/Mesh.hpp"
#include "parser/Element.hpp"

namespace E3D::Partition {
	struct SU2Mesh {
		// Dimensions de la partition
		int NDIM;
		int NELEM;
		int NPOIN;
		int ID;

		// Connectivité elements - nodes
		std::vector<int> elem2nodeStart;
		std::vector<int> elem2node;

		// Interfaces avec les autres partitions
		std::vector<int> Ninterface;
		std::vector<int> Ninterface_elem;
		std::vector<std::vector<int>> interface_elem;
		std::vector<std::vector<int>> interface_elemStart;


		// Conditions limites du maillage global
		// E3D::Parser::BC_Structure Markers;
		std::unordered_map<std::string, std::vector<E3D::Parser::Element>> Markers;

		/**
         * @brief Add an element to Markers member variable
         * 
         * @param tag marker tag
         * @param VTKid element VTK id number
         * @param elem2Node pointer to the first element of the elem2Node array
         * @param nNode number of nodes in the elem2Node array
         */
		void AddMarkerElement(const std::string &tag, int VTKid, int *elem2Node, int nNode);

		/**
         * @brief obatin the global node ID from the local ID
         * 
         * @param localID 
         * @return int 
         */
		inline int LocalNode2global(int localID) {
			return _localNode2globalPtr[localID];
		}

		/**
         * @brief Set the Local 2 Global Connectivy used in LocalNode2global function
         * 
         * @details This function will only save a reference to the correct
         *      location in localNode2Global. The input vector must stay const
         * 
         * @param localNode2Global 
         * @param localNode2GlobalStart 
         */
		void SetLocal2GlobalConnectivy(const std::vector<int> &localNode2Global,
		                               const std::vector<int> &localNode2GlobalStart);

	private:
		const int *_localNode2globalPtr;
	};

	class Partition {
	private:
		E3D::Mesh<E3D::Parser::SU2MeshParser> *_m_meshGlobal;//   Mesh object to be partitionned, connectivity must already be solved

		int _m_nPart;                      //   Nombre de partitions
		std::vector<idx_t> _m_elem2Part;//   i position holds the partition number for global element number i (0 based)
		std::vector<int> _m_Part2Elem;     //   contain the global element indexes for each partition in continuous blocks
		std::vector<int> _m_Part2ElemStart;//   contain the start index of each block associated to a partition in _m_Part2Elem

		std::vector<int> _m_nElemPerPart;//   number of elements in each partition
		std::vector<int> _m_nNodePerPart;//   number of nodes in each partition

		// linked list to go from local to globalindex
		// _m_localNode2Global contains the global index for local node i at position i for each partition in continuous blocks
		// _m_localNode2GlobalStart contains the start index for each partition in _m_localNode2Global
		std::vector<int> _m_localNode2Global;
		std::vector<int> _m_localNode2GlobalStart;

		// contain the local index in the partition for each global element index.
		// This vector need to be linked with _m_elem2Part in order to obtain the index of the partition
		std::vector<int> _m_globalElem2Local;
		std::vector<int> _m_globalNode2Local;

		// Vecteur contenant les partitions
		std::vector<SU2Mesh> _m_part;

		// Connectivité element noeud pour l'appel de METIS
		std::vector<idx_t> _m_elem2Node;
		std::vector<idx_t> _m_elem2NodeStart;

		/**
         * Résolution de la connectivité Element vers Partition
         *
         * La fonction fait appel à METIS et récupère le vecteur de connectivité 
         * Element vers Partition.
         */
		void SolveElem2Part();
		/**
         * Résolution de la connectivité Partition vers Element
         *
         * La fonction calcule d'abord le nombre d'éléments associés à chaque partition.
         * Ensuite, la connectivité Partition vers Element est générée sous la forme de
         * linked-list.
         */
		void SolvePart2Elem();
		/**
         * Conversion des noeuds locaux vers les noeuds globaux
         *
         * @param[in]   iPart   index d'une partition
         * @param[in]   nodeGlobal  index global du noeud
         * @return  index local du noeud dans la partition
         */
		int Global2Local(int &iPart, int &nodeGlobal);
		/**
         * Conversion des noeuds  globaux vers les noeuds locaux 
         *
         */
		int globalNode2Local(int GlobNode, int iPart);
		/**
         * Résout la connectivité element vers noeuds pour chaque partition
         *
         */
		void SolveElem2Node();
		/**
         * Résout la connectivité des interfaces entre les partitions.
         * 
         * La fonction établit,pour chaque partition, les interfaces entre les autres
         * partitions du domaine. Elle permet aussi de faire le traitement des conditions
         * limites initiales présentes dans le maillage global.
         */
		void SolveBorder();

		/**
         * Écrit la partition dans un fichier Tecplot
         * 
         * @param[in]   fileName   Nom du fichier Tecplot
         */
		void WriteTecplot(const std::string &fileName);

		/**
         * @brief Transforms the SU2Mesh data structure into a compatible structure
         *      for the write 
         * 
         * @param partition SU2Mesh object to be written to file
         * @param path Save path
         */
		void WriteSU2(E3D::Partition::SU2Mesh &partition, const std::string &path);

	public:
		/** 
         * @brief       Constructor 
         *
         * @param[in]   meshGlobal      mesh to be partitionned with solved connectivity
         * @param[in]   nPart           number of mesh partitions required
         *
         */
		Partition(Mesh<E3D::Parser::SU2MeshParser> *meshGlobal, int &nPart);
		/**
         * Destructeur de la classe
         *
         */
		~Partition();

		/**
         * @brief Partition the global mesh and write to file
         * 
         * @param SU2OuputPath Save path for the partitions, the # symbol will
         *      be replaced by the partition ID
         */
		void Write(const std::vector<std::string> &SU2OuputPath);

		/**
         * @brief Find the global node ID from a local node ID and part ID
         * 
         * @param localNodeID 
         * @param partID 
         * @return int global node ID
         */
		int Local2GlobalNode(int localNodeID, int partID);
	};
}// namespace E3D::Partition
