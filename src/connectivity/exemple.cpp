#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "Connectivity.cpp"

int nElem = 4;
int nNode = 15;


int main(){

  int element2nodeStart[nElem+1] = {0,5,13,19,27};
  int element2node[element2nodeStart[nElem]] = {9,13,5,14,8,12,6,7,4,9,13,5,14,10, 12, 9, 11, 4, 14, 0, 2, 3, 1, 10, 12, 4, 11};

  //std::vector<int> element2nodeStart;
  //element2nodeStart.resize(nElem+1);


  for (size_t i = 0; i < nElem+1; i++) {
    printf("%2d ", element2nodeStart[i] );
  }

  std::cout << '\n';

  for (size_t i = 0; i < element2nodeStart[nElem]; i++) {
    printf("%2d ", element2node[i] );
  }




SolveNode2element(element2nodeStart, element2node, nElem, nNode);
//SolveElement2Element(nElem, element2node, element2nodeStart, nNode, node2element, node2elementStart);
int VTK[nElem] = {14,12,13,12};
  //element2element
ComputeVTKLinkedLists(VTK, 4);


  // il faut changer les variables pour des pointeurs et ecrire le output des fonctions dans mesh.


}
