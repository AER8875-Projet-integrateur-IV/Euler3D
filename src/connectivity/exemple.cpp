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
  printf("element2nodeStart = \n");

  for (size_t i = 0; i < nElem+1; i++) {
    printf("%2d ", Connec._element2nodeStart[i] );
  }

  std::cout << '\n';
  printf("element2node = \n");
  for (size_t i = 0; i < Connec._element2nodeStart[Connec._nElem]; i++) {
    printf("%2d ", Connec._element2node[i] );
  }
  std::cout << '\n';



  Connec.ComputeVTKLinkedLists(VTK);

  std::cout  << '\n';
  Connec.SolveNode2element();
  printf("node2elementStart = \n");
  for (size_t i = 1; i < Connec._node2elementStart.size(); i++) {
    printf("%2d ", Connec._node2elementStart[i] );
  }
  std::cout << '\n';
  printf("node2element = \n");
  for (size_t i = 0; i < Connec._node2element.size(); i++) {
    printf("%2d ", Connec._node2element[i] );
  }

  int BoundaryFaces  = 16;//aller chercher ca avec le parser
  Connec.SolveElement2Element(VTK, BoundaryFaces);
  printf("element2elementStart = \n");
  for (size_t i = 0; i < Connec._nElem+1; i++) {
    printf("%2d ", Connec._element2elementStart[i] );
  }
  std::cout << '\n';
  printf("element2element = \n");
  for (size_t i = 0; i < Connec._element2elementStart[Connec._nElem]; i++) {
    printf("%2d ", Connec._element2element[i] );
  }
  std::cout << '\n';
  printf("element2faceStart = \n");
  for (size_t i = 0; i < Connec._nElem+1; i++) {
    printf("%2d ", Connec._element2faceStart[i] );
  }
  std::cout << '\n';
  printf("element2face = \n");
  for (size_t i = 0; i < Connec._element2faceStart[Connec._nElem]; i++) {
    printf("%2d ", Connec._element2face[i] );
  }
  std::cout << '\n';

  printf("face2element = \n");
  for (size_t i = 0; i < Connec._face2element.size(); i++) {
    printf("%2d ", Connec._face2element[i] );
  }
  std::cout << '\n';


  printf("face2nodeStart = \n");
  for (size_t i = 0; i < Connec._face2nodeStart.size(); i++) {
    printf("%2d ", Connec._face2nodeStart[i] );
  }
  std::cout << '\n';

  printf("face2node = \n");
  for (size_t i = 0; i < Connec._face2node.size(); i++) {
    printf("%2d ", Connec._face2node[i] );
  }
  std::cout << '\n';





  // il faut changer les variables pour des pointeurs et ecrire le output des fonctions dans mesh.


}
