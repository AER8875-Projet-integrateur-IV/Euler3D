
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

#include <iostream>
#include <string>
#include <vector>

class Mesh
{
};
class Marker
{
};

class Partition
{
private:
    Mesh *_m_meshGlobal; //   Mesh object to be partitionned, connectivity must already be solved  

    int _m_nPart; //   Nombre de partitions  
    std::vector<int> _m_elem2Part; //   i position holds the partition number for global element number i (0 based) 
    std::vector<int> _m_Part2Elem; //   contain the global element indexes for each partition in continuous blocks  
    std::vector<int> _m_Part2ElemStart; //   contain the start index of each block associated to a partition in _m_Part2Elem  

    std::vector<int> _m_nElemPerPart; //   number of elements in each partition  
    std::vector<int> _m_nNodePerPart; //   number of nodes in each partition  

    // linked list to go from local to globalindex
    // _m_localNode2Global contains the global index for local node i at position i for each partition in continuous blocks
    // _m_localNode2GlobalStart contains the start index for each partition in _m_localNode2Global
    std::vector<int> _m_localNode2Global; 
    std::vector<int> _m_localNode2GlobalStart;

    // same as _m_localNode2Global and _m_localNode2GlobalStart
    std::vector<int> _m_localElem2Global;
    std::vector<int> _m_localElem2GlobalStart;

    std::vector<Mesh> _m_part;  
    std::vector<Marker> _m_marker;

public:
    /** 
     * @brief       Constructor 
     *
     * @param[in]   meshGlobal      mesh to be partitionned with solved connectivity
     * @param[in]   nPart           number of mesh partitions required
     *
     */ 
    Partition(Mesh *meshGlobal, int &nPart);

    void Write();
    void SolveElem2Node();
    void Global2Local();
    void SolveBorder();
};
