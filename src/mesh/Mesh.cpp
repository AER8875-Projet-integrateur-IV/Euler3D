#include "mesh/Mesh.hpp"
using namespace E3D;

Mesh::Mesh(const std::string& fileName) : _parser(fileName) {}

void Mesh::solveConnectivity() {
    // Inititalize connectivity object


    // Call connectivity methods to solve and populate connectivity vectors (Remplir ici) :
    connectivityObj.SolveElement2node(this->GetInteriorElementVector());
    std::cout << '\n';
    connectivityObj.ComputeVTKLinkedLists(this->GetInteriorVTKID());
    connectivityObj.SolveNode2element();
    connectivityObj.SolveElement2Element(this->GetInteriorVTKID(), this->GetMeshBoundaryElemCount());
    //
    //
    //


    // Filling private member variables related to connectivity
    node2element = std::make_unique<std::vector<int>>(connectivityObj._node2element);
    node2elementStart = std::make_unique<std::vector<int>>(connectivityObj._node2elementStart);
    element2element = std::make_unique<std::vector<int>>(connectivityObj._element2element);
    element2elementStart = std::make_unique<std::vector<int>>(connectivityObj._element2elementStart);
    element2face = std::make_unique<std::vector<int>>(connectivityObj._element2face);
    element2faceStart = std::make_unique<std::vector<int>>(connectivityObj._element2faceStart);
    face2element = std::make_unique<std::vector<int>>(connectivityObj._face2element);
    face2node = std::make_unique<std::vector<int>>(connectivityObj._face2node);
    face2nodeStart = std::make_unique<std::vector<int>>(connectivityObj._face2nodeStart);


}
