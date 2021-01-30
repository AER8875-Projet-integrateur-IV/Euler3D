#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include "parser/Element.hpp"

class Connectivity {
public:
  void SolveNode2element(const std::vector<E3D::Parser::Element> &elemVector);
  void SolveElement2Element();
  const std::vector<int> &GetNodeIndices(int face, int jelem, int nLocalFacefElement, int VTK);
  void ComputeVTKLinkedLists(const std::vector<int> &VTK);

  std::vector<int> _node2element;
  std::vector<int> _node2elementStart;
  std::vector<int> _element2element;
  std::vector<int> _element2elementStart;
  std::vector<int> _element2face;
  std::vector<int> _element2faceStart;
  std::vector<int> _face2element;
  std::vector<int> _face2elementStart;
  std::vector<int> _nFacefElement;
  std::vector<std::vector<int>> _nNodefFace;
};
