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
    Mesh *_m_meshGlobal;

    int _m_nPart; // Nombre de partitions
    std::vector<int> _m_elem2Part;
    std::vector<int> _m_Part2Elem;
    std::vector<int> _m_Part2ElemStart;

    std::vector<int> _m_nElemPerPart;
    std::vector<int> _m_nNodePerPart;

    std::vector<int> _m_localNode2Global;
    std::vector<int> _m_localNode2GlobalStart;

    std::vector<int> _m_localElem2Global;
    std::vector<int> _m_localElem2GlobalStart;

    std::vector<Mesh> _m_part;
    std::vector<Marker> _m_marker;

public:
    Partition(Mesh *meshGlobal, int &nPart);

    void Write();
    void SolveElem2Node();
    void Global2Local();
    void SolveBorder();
};