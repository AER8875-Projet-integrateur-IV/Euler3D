#include "connectivity/Connectivity.hpp"


void Connectivity::SolveNode2element(const std::vector<E3D::Parser::Element> &elemVector){
  std::vector<int> node2elementStart;
  std::vector<int> node2element;

  std::vector<int> element2node;
  int nElem = elemVector.size();
  std::vector<int> element2nodeStart(nElem+1,0);
  int counter = 0;
  for (size_t ielem = 0; ielem < nElem; ielem++) {
    E3D::Parser::Element elem = elemVector[ielem];
    for(int const &node : elem.getElemNodes()){
      counter++;
    }
    element2nodeStart[ielem+1] = counter;
  }
  element2node.resize(counter);
  int nNode = counter;

  //construire element2nodeStart
  for(E3D::Parser::Element const &elem : elemVector){

    for(int const &node : elem.getElemNodes()){
      element2node.push_back(node);
    }
  }




  node2elementStart.resize(nNode+1);
  node2element.resize(element2nodeStart[nElem]);



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



}

void Connectivity::SolveElement2Element(int nElem, int element2node[], int element2nodeStart[],
  int nNode, int node2element[], int node2elementStart[], int VTK[],
  std::vector<int> nFacefElement, std::vector<std::vector<int>> nNodefFace){
    
  std::vector<int> element2faceStart;
  std::vector<int> element2elementStart;
  element2faceStart.resize(nElem+1);
  element2elementStart.resize(nElem+1);

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
  std::vector<int> face2node;






    int nLocalFacefElement;
    int faceCount = 0;
    int nodeCount = 0;


  for (size_t ielem = 0; ielem < nElem; ielem++) {
    int startI = element2nodeStart[ielem];
    int endI = element2nodeStart[ielem+1];
    if (VTK[ielem] == 12) {
      //methode pour cube
      nLocalFacefElement = nFacefElement[ielem];

      int lhelp_indice[24] = {0,3,2,1,0,1,5,4,1,2,6,5,2,3,7,6,3,0,4,7,4,5,6,7};
      int lhelp_indiceStart[nLocalFacefElement+1] = {0,4,8,12,16,20,24};
      for (size_t iface = 0; iface < nLocalFacefElement; iface++) {
        int nLocalNodefFaceI = nNodefFace[ielem][iface];
        int lhelp[nLocalNodefFaceI] = {0};
        int lpoint[nNode] = {0};
        for (size_t inode = 0; inode < nLocalNodefFaceI; inode++) {
          lhelp[inode] = element2node[startI+lhelp_indice[lhelp_indiceStart[iface]+inode]];
          lpoint[lhelp[inode]]  = 1;
        }
        int ipoint = lhelp[0];

        int elemStart = node2elementStart[ipoint];
        int elemEnd = node2elementStart[ipoint+1];

        for (size_t j = elemStart; j < elemEnd; j++) {
          int jelem = node2element[j];

          if (jelem != ielem) {
            int startJ = element2nodeStart[jelem];
            int endJ = element2nodeStart[jelem+1];
            int nLocaleFacesJ = nFacefElement[jelem];

            for (size_t jface = 0; jface < nLocaleFacesJ; jface++) {
              int nNodeForFaceJ = nNodefFace[jelem][jface];
              if (nLocalNodefFaceI == nNodeForFaceJ) {
                int count = 0;
                std::vector<int> nodeIndice = GetNodeIndices(jface, jelem, nLocaleFacesJ, VTK[jelem]);
                for (size_t JlocalNode = 0; JlocalNode < nNodeForFaceJ; JlocalNode++) {
                  int pointIndex = startJ + nodeIndice[JlocalNode];

                  // if (facej == (nLocaleFacesJ - 1) && localNodeJ == (nNodesForFaceJ - 1)) {
  								// 	pointIndex = startJ;
  								// } // a voir si besoin
                  count += lpoint[element2node[pointIndex]];

                  if (count == nLocalNodefFaceI) {
                    element2element[element2elementStart[ielem]+ iface] = jelem;
                  }
                  if (element2element[element2elementStart[jelem] + jface] == ielem) {
                    element2face[element2elementStart[ielem] + iface] = element2face[element2elementStart[jelem] + jface];
                  }
                  else {
                    element2face[element2elementStart[ielem] + iface] = faceCount;
                    element2face[element2elementStart[jelem] + jface] = faceCount;
                    face2element[2*faceCount] = ielem;
                    face2element[2*faceCount +1] = jelem;
                    for (size_t i = 0; i < nLocalNodefFaceI; i++) {
                      //face2node[nodeCount] = lhelp[i];
                      nodeCount +=1;
                    }
                    faceCount++;
                  }
                }
              }
            }


          }
        }
        for (size_t i = 0; i < nLocalNodefFaceI; i++) {
          lpoint[lhelp[i]] = 0;
        }
      }


    }
    else if (VTK[ielem] == 13) {
      //methode pour wedge
      nLocalFacefElement = nFacefElement[ielem];

      int lhelp_indice[18] = {0,1,2,0,2,5,3,0,3,4,1,1,4,5,2,3,5,4};
      int lhelp_indiceStart[nLocalFacefElement+1] = {0,3,7,11,15,18};

    }
    else if (VTK[ielem] == 14) {
      //methode pour pyramide base carree
      nLocalFacefElement = nFacefElement[ielem];

      int lhelp_indice[16] = {0,3,2,1,0,1,4,1,2,4,2,3,4,3,0,4};
      int lhelp_indiceStart[nLocalFacefElement+1] = {0,4,7,10,13,16};

    }
    else if (VTK[ielem] == 10) {
      //methode pour tetrahedre
      nLocalFacefElement = nFacefElement[ielem];

      int lhelp_indice[12] = {0,2,1,0,1,3,1,2,3,2,0,3};
      int lhelp_indiceStart[nLocalFacefElement+1] = {0,3,6,9,12};

    }
    else {
      std::cout << "VTK not recognised!" << '\n';
    }
  }
}

const std::vector<int> &Connectivity::GetNodeIndices(int face, int jelem, int nLocalFacefElement, int VTK){
 // enlever la redondance entre le VTKjelem et jelem
  int VTKjelem = VTK;
  std::vector<int> indiceNodes;
  if (VTKjelem == 10) {
    int lhelp_indice[12] = {0,2,1,0,1,3,1,2,3,2,0,3};
    int lhelp_indiceStart[nLocalFacefElement+1] = {0,3,6,9,12};
    int start = lhelp_indiceStart[face];
    int end = lhelp_indiceStart[face+1];
    indiceNodes.resize(end-start);

    for (size_t i = start; i < end; i++) {
      indiceNodes[i-start] = lhelp_indice[i];
      printf("%d ", indiceNodes[i-start]);
    }
  }
  else if (VTKjelem == 12) {
    int lhelp_indice[24] = {0,3,2,1,0,1,5,4,1,2,6,5,2,3,7,6,3,0,4,7,4,5,6,7};
    int lhelp_indiceStart[nLocalFacefElement+1] = {0,4,8,12,16,20,24};
    int start = lhelp_indiceStart[face];
    int end = lhelp_indiceStart[face+1];
    indiceNodes.resize(end-start);

    for (size_t i = start; i < end; i++) {
      indiceNodes[i-start] = lhelp_indice[i];
      printf("%d ", indiceNodes[i-start]);
    }
  } else if (VTKjelem == 13) {
    int lhelp_indice[18] = {0,1,2,0,2,5,3,0,3,4,1,1,4,5,2,3,5,4};
    int lhelp_indiceStart[nLocalFacefElement+1] = {0,3,7,11,15,18};
    int start = lhelp_indiceStart[face];
    int end = lhelp_indiceStart[face+1];
    indiceNodes.resize(end-start);

    for (size_t i = start; i < end; i++) {
      indiceNodes[i-start] = lhelp_indice[i];
      printf("%d ", indiceNodes[i-start]);
  }
  } else if (VTKjelem == 14) {
    int lhelp_indice[16] = {0,3,2,1,0,1,4,1,2,4,2,3,4,3,0,4};
    int lhelp_indiceStart[nLocalFacefElement+1] = {0,4,7,10,13,16};
    int start = lhelp_indiceStart[face];
    int end = lhelp_indiceStart[face+1];
    indiceNodes.resize(end-start);

    for (size_t i = start; i < end; i++) {
      indiceNodes[i-start] = lhelp_indice[i];
      printf("%d ", indiceNodes[i-start]);
    }

  }
  return indiceNodes;
}

void Connectivity::ComputeVTKLinkedLists(const std::vector<int> &VTK){

  std::vector<int> nFacefElement;
  std::vector<std::vector<int>> nNodefFace;
  int nElem = VTK.size();
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
    else {
      std::cout << "vtk id not recognised" << '\n';
    }
  }
}
