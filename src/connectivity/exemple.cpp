#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "Connectivity.cpp"


int nElem = 4;
int nNode = 15;


int main(){
  Connectivity Connec;
  Connec._element2nodeStart = {0,5,13,19,27};
  Connec._element2node = {9,13,5,14,8,12,6,7,4,9,13,5,14,10, 12, 9, 11, 4, 14, 0, 2, 3, 1, 10, 12, 4, 11};
  //std::vector<int> VTK = mesh.GetInteriorVTKID();
  std::vector<int> VTK = {14,12,13,12};
  //std::vector<int> element2nodeStart;
  //element2nodeStart.resize(nElem+1);


  Connec._nElem = 4;// remplace le call de solve element2node
  Connec._nNode = 15;
  
  for (size_t i = 0; i < nElem+1; i++) {
    printf("%2d ", Connec._element2nodeStart[i] );
  }

  std::cout << '\n';

  for (size_t i = 0; i < Connec._element2nodeStart[Connec._nElem]; i++) {
    printf("%2d ", Connec._element2node[i] );
  }
  std::cout << '\n';



  Connec.ComputeVTKLinkedLists(VTK);
  for (size_t i = 0; i < Connec._nNodefFace.size(); i++) {
    std::cout << '\n';
    for (size_t j = 0; j < Connec._nNodefFace[i].size(); j++) {
      printf("%2d ", Connec._nNodefFace[i][j] );
    }
  }
  std::cout  << '\n';
  Connec.SolveNode2element();
  Connec.SolveElement2Element(VTK);
  for (size_t i = 0; i < Connec._nElem+1; i++) {
    printf("%2d ", Connec._element2elementStart[i] );
  }
  std::cout << '\n';
  for (size_t i = 0; i < Connec._element2elementStart[Connec._nElem]; i++) {
    printf("%2d ", Connec._element2element[i] );
  }
  std::cout << '\n';
//SolveElement2Element(nElem, element2node, element2nodeStart, nNode, node2element, node2elementStart);




  // il faut changer les variables pour des pointeurs et ecrire le output des fonctions dans mesh.


}
