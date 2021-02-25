#include "mesh/Mesh.hpp"
using namespace E3D;

Mesh::Mesh(const std::string& fileName) : _parser(fileName) {
    _parser.printInfo();
}

void Mesh::solveConnectivity() {
    // Inititalize connectivity object


    // Call connectivity methods to solve and populate connectivity vectors (Remplir ici) :
    connectivityObj._nNode = GetMeshNodeCount();
    connectivityObj.SolveElement2node(GetInteriorElementVector());





    connectivityObj.ComputeVTKLinkedLists(GetInteriorVTKID());
    connectivityObj.SolveNode2element();
    connectivityObj.SolveElement2Element(GetInteriorVTKID(), GetMeshBoundaryElemCount());
    //
    //
    //


    // Filling private member variables related to connectivity
	nFace = connectivityObj._nFace;
	nElemTot = connectivityObj._nElem;
	node2element = std::make_unique<std::vector<int>>(connectivityObj._node2element);
    node2elementStart = std::make_unique<std::vector<int>>(connectivityObj._node2elementStart);
    element2element = std::make_unique<std::vector<int>>(connectivityObj._element2element);
    element2elementStart = std::make_unique<std::vector<int>>(connectivityObj._element2elementStart);
    element2face = std::make_unique<std::vector<int>>(connectivityObj._element2face);
    element2faceStart = std::make_unique<std::vector<int>>(connectivityObj._element2faceStart);
    face2element = std::make_unique<std::vector<int>>(connectivityObj._face2element);
    face2node = std::make_unique<std::vector<int>>(connectivityObj._face2node);
    face2nodeStart = std::make_unique<std::vector<int>>(connectivityObj._face2nodeStart);

    //printing linked lists
    // printf("Element2nodeStart = ");
    // for (size_t i = 0; i < connectivityObj._element2nodeStart.size(); i++) {
    //   printf("%2d ", connectivityObj._element2nodeStart[i] );
    // }
    // printf("\n Element2node = ");
    // for (size_t i = 0; i < connectivityObj._element2node.size(); i++) {
    //   printf("%2d ", connectivityObj._element2node[i] );
    // }
    //
    // printf("\n node2elementStart = ");
    // for (size_t i = 0; i < connectivityObj._node2elementStart.size(); i++) {
    //   printf("%2d ", connectivityObj._node2elementStart[i] );
    // }
    // printf("\n node2element = ");
    // for (size_t i = 0; i < connectivityObj._node2element.size(); i++) {
    //   printf("%2d ", connectivityObj._node2element[i] );
    // }
    //
    // printf("\n element2elementStart = ");
    // for (size_t i = 0; i < connectivityObj._element2elementStart.size(); i++) {
    //   printf("%2d ", connectivityObj._element2elementStart[i] );
    // }
    // printf("\n element2element = ");
    // for (size_t i = 0; i < connectivityObj._element2element.size(); i++) {
    //   printf("%2d ", connectivityObj._element2element[i] );
    // }
    //
    // printf("\n element2faceStart = ");
    // for (size_t i = 0; i < connectivityObj._element2faceStart.size(); i++) {
    //   printf("%2d ", connectivityObj._element2faceStart[i] );
    // }
    // printf("\n element2face = ");
    // for (size_t i = 0; i < connectivityObj._element2face.size(); i++) {
    //   printf("%2d ", connectivityObj._element2face[i] );
    // }
    //
    // printf("\n face2element = ");
    // for (size_t i = 0; i < connectivityObj._face2element.size(); i++) {
    //   printf("%2d ", connectivityObj._face2element[i] );
    // }
    //
    // printf("\n face2nodeStart = ");
    // for (size_t i = 0; i < connectivityObj._face2nodeStart.size(); i++) {
    //   printf("%2d ", connectivityObj._face2nodeStart[i] );
    // }
    // printf("\n face2node = ");
    // for (size_t i = 0; i < connectivityObj._face2node.size(); i++) {
    //   printf("%2d ", connectivityObj._face2node[i] );
    // }
}
