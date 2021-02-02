
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

#include "mesh/Mesh.hpp"
#include "parser/Element.hpp"

namespace E3D::Partition {
	struct SU2Mesh {
		// Dimensions de la partition
		int NDIM;
		int NELEM;
		int NPOIN;

		// Connectivité elements - nodes
		std::vector<int> elem2nodeStart;
		std::vector<int> elem2node;

		// Interfaces avec les autres partitions
		std::vector<int> Ninterface;
		std::vector<int> Ninterface_elem;
		std::vector<std::vector<int>> interface_elem;

		// Conditions limites du maillage global
		// E3D::Parser::BC_Structure Markers;
		std::unordered_map<std::string, std::vector<E3D::Parser::Element>> Markers;

		// Coordonnées des noeuds
		std::vector<int> nodeGlob;

		/**
         * @brief Add an element to Markers member variable
         * 
         * @param tag marker tag
         * @param VTKid element VTK id number
         * @param elem2Node pointer to the first element of the elem2Node array
         * @param nNode number of nodes in the elem2Node array
         */
		void AddMarkerElement(std::string tag, int VTKid, int *elem2Node, int nNode);
	};

	class Partition {
	private:
		E3D::Mesh *_m_meshGlobal;//   Mesh object to be partitionned, connectivity must already be solved

		int _m_nPart;                      //   Nombre de partitions
		std::vector<long int> _m_elem2Part;//   i position holds the partition number for global element number i (0 based)
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

		// Vecteur contenant les partitions
		std::vector<SU2Mesh> _m_part;

		// Connectivité element noeud pour l'appel de METIS
		std::vector<long int> _m_elem2Node;
		std::vector<long int> _m_elem2NodeStart;

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
		void WriteTecplot(std::string fileName);

		void WriteSU2(E3D::Partition::SU2Mesh const &partition);

	public:
		/** 
         * @brief       Constructor 
         *
         * @param[in]   meshGlobal      mesh to be partitionned with solved connectivity
         * @param[in]   nPart           number of mesh partitions required
         *
         */
		Partition(Mesh *meshGlobal, int &nPart);
		/**
         * Destructeur de la classe
         *
         */
		~Partition();

		void Write();
	};
};// namespace E3D::Partition

/* 
element2NodeStart = [0, 5, 13, 19, 27]
element2Node = [9, 13, 5, 14, 8, 12, 6, 7, 4, 9, 13, 5, 14, 10, 12, 9, 11, 4, 14, 0, 2, 3, 1, 10, 12, 4, 11]

node2ElementStart = [0, 1, 2, 3, 4, 7, 9, 10, 11, 12, 15, 17, 19, 22, 24, 27]
node2Element = [3, 3, 3, 3, 1, 2, 3, 0, 1, 1, 1, 0, 0, 1, 2, 2, 3, 2, 3, 1, 2, 3, 0, 1, 0, 1, 2]

element2FaceStart = [0, 5, 11, 16, 22]
element2Face = [0, 3, 4, 5, 6, 0, 1, 7, 8, 9, 10, 1, 2, 11, 12, 13, 2, 14, 15, 16, 17, 18]
face2Element = [0, 1, 1, 2, 2, 3, 0, 4, 0, 5, 0, 6, 0, 7, 1, 10, 1, 9, 1, 8, 1, 11, 2, 14, 2, 12, 2, 13, 3, 17, 3, 16, 3, 15, 3, 18, 3, 19] C'est la seule connec qui ne necessite pas de Start pcq chaque face est connectee a 2 elements et seulement 2
face2NodeStart = [0, 4, 8, 12, 15, 18, 21, 24, 28, 32, 36, 40, 43, 46, 50, 54, 58, 62, 66, 70]
face2Node = [9, 13, 5, 14, 12, 4, 14, 9, 10, 12, 4, 11, 9, 13, 8, 13, 5, 8, 5, 14, 8, 14, 9, 8, 12, 6, 13, 9, 6, 7, 5, 13, 7, 4, 14, 5, 12, 6, 7, 4, 10, 12, 9, 4, 11, 14, 10, 9, 14, 11, 0, 2, 12, 10, 2, 3, 4, 12, 3, 1, 11, 4, 1, 0, 10, 11, 0, 2, 3, 1] 
*/