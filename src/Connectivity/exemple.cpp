#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

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




  std::vector<int> node2elementStart;
  std::vector<int> node2element;
  std::vector<int> element2elementStart;
  std::vector<int> element2faceStart;
  element2faceStart.resize(nElem+1);
  node2elementStart.resize(nNode+1);
  node2element.resize(element2nodeStart[nElem]);
  element2elementStart.resize(nElem+1);


  int startI;
  int endI;
  int nodeI;

  for (size_t ielem = 0; ielem < nElem; ielem++) {
    startI = element2nodeStart[ielem];
    endI = element2nodeStart[ielem+1];

    for (size_t i = startI; i < endI; i++) {
      nodeI = element2node[i];
      node2elementStart[nodeI+1] += 1;
    }

  }
  std::cout << node2elementStart[0] << " " ;
  for (size_t i = 1; i < node2elementStart.size(); i++) {
    node2elementStart[i] += node2elementStart[i-1];
    printf("%2d ", node2elementStart[i] );
  }

  int j;
  int store[nNode] = {0};
  for (size_t ielem = 0; ielem < nElem; ielem++) {
    startI = element2nodeStart[ielem];
    endI = element2nodeStart[ielem+1];

    for (size_t i = startI; i < endI; i++) {
      nodeI = element2node[i];
      j = node2elementStart[nodeI] + store[nodeI];
      node2element[j] = ielem;
      store[nodeI]++;

    }

  }
  std::cout << '\n';
  for (size_t i = 0; i < node2element.size(); i++) {
    printf("%2d ", node2element[i] );
  }


  int nlocalfaces[4] = {5,6,5,6};
  std::cout << '\n' <<element2faceStart[0] << " ";
  for (size_t i = 1; i < nElem+1; i++) {
    element2elementStart[i] = nlocalfaces[i-1];
    element2elementStart[i] += element2elementStart[i-1];
    element2faceStart[i] = element2elementStart[i];
    printf("%2d ", element2faceStart[i]);
  }

  int BoundaryFaces  = 16;//aller chercher ca avec le parser

  int nFaces = (BoundaryFaces + element2elementStart[nElem])/2;
  std::cout << '\n' <<nFaces << '\n';

  std::vector<int> element2element(element2elementStart[nElem],-1);
  std::vector<int> element2face(element2elementStart[nElem],-1);
  std::vector<int> face2element(nFaces*2,-1);
  //verifier l'ordre des faces pour chaque element, ex. dans une pyramide, la base est-elle toujours a l'indice 0?

}

void Solvenode2elementStart(){

}
