#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include "parser/Element.hpp"

class Connectivity {
public:
  //Connectivity(E3D::Parser::Mesh &mesh):_mesh(mesh){};
  void SolveElement2node(const std::vector<E3D::Parser::Element> &elemVector);
  void SolveNode2element();
  void SolveElement2Element(const std::vector<int> &VTK, int BoundaryFaces);
  const std::vector<int> GetNodeIndices(int face, int VTK);
  void ComputeVTKLinkedLists(const std::vector<int> &VTK);

  int _nElem;
  int _nNode;
  int _nFace;
  int _nFaceInt;
  std::vector<int> _element2node;
  std::vector<int> _element2nodeStart;
  std::vector<int> _node2element;
  std::vector<int> _node2elementStart;
  std::vector<int> _element2element;
  std::vector<int> _element2elementStart;
  std::vector<int> _element2face;
  std::vector<int> _element2faceStart;
  std::vector<int> _face2element;
  std::vector<int> _face2node;
  std::vector<int> _face2nodeStart;
  std::vector<int> _nFacefElement;
  std::vector<std::vector<int>> _nNodefFace;

private:
  //E3D::Parser::Mesh _mesh;
  const std::vector<std::vector<int>> _lhelp_indice{{0, 2, 1, 0, 1, 3, 1, 2, 3, 2, 0, 3},
                                                {0, 3, 2, 1, 0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6, 3, 0, 4, 7, 4, 5, 6, 7},
                                                {0, 1, 2, 0, 2, 5, 3, 0, 3, 4, 1, 1, 4, 5, 2, 3, 5, 4},
                                                {0, 3, 2, 1, 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4}};

  const std::vector<std::vector<int>> _lhelp_indiceStart{{0, 3, 6, 9,  12},
                                                     {0, 4, 8, 12, 16, 20, 24},
                                                     {0, 3, 7, 11, 15, 18},
                                                     {0, 4, 7, 10, 13, 16}};


};
