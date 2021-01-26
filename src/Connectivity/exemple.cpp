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


  //node2element

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

  //element2element


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




  int VTK[nElem] = {14,12,13,12};
  std::vector<int> nFacefElement;
  std::vector<std::vector<int>> nNodefFace;
  nFacefElement.resize(nElem);
  nNodefFace.resize(nElem);

  for (size_t i = 0; i < nElem; i++) {
    if (VTK[i] == 10) {
      nFacefElement[i] = 4;
      nNodefFace[i].resize(nFacefElement[i]);
      nNodefFace[i][0] = 3;
      nNodefFace[i][1] = 3;
      nNodefFace[i][2] = 3;
      nNodefFace[i][3] = 3;

    }
    else if (VTK[i] == 12) {
      nFacefElement[i] = 6;
      nNodefFace[i].resize(nFacefElement[i]);
      nNodefFace[i][0] = 4;
      nNodefFace[i][1] = 4;
      nNodefFace[i][2] = 4;
      nNodefFace[i][3] = 4;
      nNodefFace[i][4] = 4;
      nNodefFace[i][5] = 4;

    }
    else if (VTK[i] == 13) {
      nFacefElement[i] = 5;
      nNodefFace[i].resize(nFacefElement[i]);
      nNodefFace[i][0] = 3;
      nNodefFace[i][1] = 4;
      nNodefFace[i][2] = 4;
      nNodefFace[i][3] = 4;
      nNodefFace[i][4] = 3;
    }
    else if (VTK[i] == 14) {
      nFacefElement[i] = 5;
      nNodefFace[i].resize(nFacefElement[i]);
      nNodefFace[i][0] = 4;
      nNodefFace[i][1] = 3;
      nNodefFace[i][2] = 3;
      nNodefFace[i][3] = 3;
      nNodefFace[i][4] = 3;
    }
  }




  for (size_t ielem = 0; ielem < nElem; ielem++) {
    startI = element2nodeStart[ielem];
    endI = element2nodeStart[ielem+1];
    if (VTK[ielem] == 12) {
      //methode pour cube
      int nLocalFacefElement = nFacefElement[ielem];

      int lhelp_indice[24] = {0,3,2,1,0,1,5,4,1,2,6,5,2,3,7,6,3,0,4,7,4,5,6,7};
      int lhelp_indiceStart[nLocalFacefElement] = {0,4,8,12,16,20};
      for (size_t iface = 0; iface < nLocalFacefElement; iface++) {
        int nLocalNodefFace = nNodefFace[ielem][iface];
        int lhelp[nLocalNodefFace[iface]] = {0};
        int lpoint[nNode] = {0};
        for (size_t inode = 0; inode < nLocalNodefFace[iface]; inode++) {
          lhelp[inode] = element2node[startI+lhelp_indice[lhelp_indiceStart[iface]+inode]];
          lpoint[lhelp[inode]]  = 1;
        }
        int ipoint = lhelp[0];

        int elemStart = node2elementStart[ipoint];
        int elemEnd = node2elementStart[ipoin+1];

        for (size_t j = elemStart; j < elemEnd; j++) {
          jelem = node2element[j];

          if (jelem != ielem) {
            startJ = element2nodeStart[jelem];
            endJ = element2nodeStart[jelem+1];


          }
        }

      }


    }
    else if (VTK[ielem] == 13) {
      //methode pour wedge

    }
    else if (VTK[ielem] == 14) {
      //methode pour pyramide base carree

    }
    else if (VTK[ielem] == 10) {
      //methode pour tetrahedre

    }
    else {
      std::cout << "VTK not recognised!" << '\n';
    }
  }

  //verifier l'ordre des faces pour chaque element, ex. dans une pyramide, la base est-elle toujours a l'indice 0?

}

void Solvenode2elementStart(){

}
