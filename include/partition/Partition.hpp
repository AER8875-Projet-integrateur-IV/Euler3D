
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
#include <vector>

#include "parser/Element.hpp"
#include "mesh/Mesh.hpp"

namespace E3D::Partition
{
    struct SU2Mesh
    {
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
        std::vector<std::vector<int>> boundaryStart;
        std::vector<std::vector<int>> boundary;

        // Coordonnées des noeuds
        std::vector<int> nodeGlob;
    };

    class Partition
    {
    private:
        E3D::Mesh *_m_meshGlobal; //   Mesh object to be partitionned, connectivity must already be solved

        int _m_nPart;                       //   Nombre de partitions
        std::vector<long int> _m_elem2Part; //   i position holds the partition number for global element number i (0 based)
        std::vector<int> _m_Part2Elem;      //   contain the global element indexes for each partition in continuous blocks
        std::vector<int> _m_Part2ElemStart; //   contain the start index of each block associated to a partition in _m_Part2Elem

        std::vector<int> _m_nElemPerPart; //   number of elements in each partition
        std::vector<int> _m_nNodePerPart; //   number of nodes in each partition

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
}; // namespace E3D::Partition