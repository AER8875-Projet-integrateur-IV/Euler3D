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
  const std::vector<int> GetNodeIndices(int face,  int nLocalFacefElement, int VTK);
  void ComputeVTKLinkedLists(const std::vector<int> &VTK);

  int _nElem;
  int _nNode;
  int _nFace;
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

//private:
  //E3D::Parser::Mesh _mesh;

};
