#include "connectivity/Connectivity.hpp"


void Connectivity::SolveElement2node(const std::vector<E3D::Parser::Element> &elemVector){

    _nElem = elemVector.size();
    _element2nodeStart.resize(_nElem+1,0);
    int counter = 0;
    for (size_t ielem = 0; ielem < _nElem; ielem++) {
      E3D::Parser::Element elem = elemVector[ielem];
      for(int const &node : elem.getElemNodes()){
        counter++;
      }
      _element2nodeStart[ielem+1] = counter;
    }
    _element2node.resize(counter);
    _nNode = counter;

    //construire element2nodeStart
    for(E3D::Parser::Element const &elem : elemVector){

      for(int const &node : elem.getElemNodes()){
        _element2node.push_back(node);
      }
    }


}

void Connectivity::SolveNode2element(){

  //node2element
  _node2elementStart.resize(_nNode+1);
  _node2element.resize(_element2nodeStart[_nElem]);

  int startI;
  int endI;
  int nodeI;
  for (size_t ielem = 0; ielem < _nElem; ielem++) {


    startI = _element2nodeStart[ielem];
    endI = _element2nodeStart[ielem+1];

    for (size_t i = startI; i < endI; i++) {
      nodeI = _element2node[i];
      _node2elementStart[nodeI+1] += 1;
    }

  }
  for (size_t i = 1; i < _node2elementStart.size(); i++) {
    _node2elementStart[i] += _node2elementStart[i-1];
    //printf("%2d ", _node2elementStart[i] );
  }

  int j;
  int store[_nNode] = {0};
  for (size_t ielem = 0; ielem < _nElem; ielem++) {
    startI = _element2nodeStart[ielem];
    endI = _element2nodeStart[ielem+1];

    for (size_t i = startI; i < endI; i++) {
      nodeI = _element2node[i];
      j = _node2elementStart[nodeI] + store[nodeI];
      _node2element[j] = ielem;
      store[nodeI]++;

    }

  }




}

void Connectivity::SolveElement2Element(const std::vector<int> &VTK){


  _element2faceStart.resize(_nElem+1);
  _element2elementStart.resize(_nElem+1);

  int nlocalfaces[4] = {5,6,5,6};
  for (size_t i = 1; i < _nElem+1; i++) {
    _element2elementStart[i] = nlocalfaces[i-1];
    _element2elementStart[i] += _element2elementStart[i-1];
    _element2faceStart[i] = _element2elementStart[i];
  }

  int BoundaryFaces  = 16;//aller chercher ca avec le parser

  _nFace = (BoundaryFaces + _element2elementStart[_nElem])/2;
  std::cout << '\n' << "nFace: " <<_nFace << '\n';


  _face2element.resize(_nFace*2, -1);
  _element2face.resize(_element2elementStart[_nElem],-1);
  _element2element.resize(_element2elementStart[_nElem],-1);


  //creating lhelp_indice vector
  std::vector<std::vector<int>> _lhelp_indice{{0,2,1,0,1,3,1,2,3,2,0,3},
                                              {0,3,2,1,0,1,5,4,1,2,6,5,2,3,7,6,3,0,4,7,4,5,6,7},
                                              {0,1,2,0,2,5,3,0,3,4,1,1,4,5,2,3,5,4},
                                              {0,3,2,1,0,1,4,1,2,4,2,3,4,3,0,4}};

  std::vector<std::vector<int>> _lhelp_indiceStart{{0,3,6,9,12},
                                                   {0,4,8,12,16,20,24},
                                                   {0,3,7,11,15,18},
                                                   {0,4,7,10,13,16}};



  int nLocalFacefElement;
  int faceCount = 0;
  int nodeCount = 0;
  int face2nodeCount = 0;

  for (size_t ielem = 0; ielem < _nElem; ielem++) {
    int startI = _element2nodeStart[ielem];
    int endI = _element2nodeStart[ielem+1];
    int ilhelp;




    // ---------- determine le type d'element ---------------
    if (VTK[ielem] == 10) {
      //methode pour tetrahedre
      nLocalFacefElement = _nFacefElement[ielem];
      face2nodeCount += 12;
      ilhelp = 0;

    }
    else if (VTK[ielem] == 12) {
      //methode pour cube
      nLocalFacefElement = _nFacefElement[ielem];
      face2nodeCount += 24;
      ilhelp = 1;

    }
    else if (VTK[ielem] == 13) {
      //methode pour wedge
      nLocalFacefElement = _nFacefElement[ielem];
      face2nodeCount += 18;
      ilhelp = 2;

    }
    else if (VTK[ielem] == 14) {
      //methode pour pyramide base carree
      nLocalFacefElement = _nFacefElement[ielem];
      face2nodeCount += 16;
      ilhelp = 3;

    }
    else {
      std::cout << "VTK not recognised!" << '\n';
    }


    // ---------- commence la connectivity element2element et face ---------------
    for (size_t iface = 0; iface < nLocalFacefElement; iface++) {
      int nLocalNodefFaceI = _nNodefFace[ielem][iface];
      int lhelp[nLocalNodefFaceI] = {0};
      int lpoint[_nNode] = {0};
      for (size_t inode = 0; inode < nLocalNodefFaceI; inode++) {
        lhelp[inode] = _element2node[startI+_lhelp_indice[ilhelp][_lhelp_indiceStart[ilhelp][iface]+inode]];
        lpoint[lhelp[inode]]  = 1;
      }
      int ipoint = lhelp[0];

      int elemStart = _node2elementStart[ipoint];
      int elemEnd = _node2elementStart[ipoint+1];

      for (size_t j = elemStart; j < elemEnd; j++) {
        int jelem = _node2element[j];
        if (jelem != ielem) {
          int startJ = _element2nodeStart[jelem];
          int endJ = _element2nodeStart[jelem+1];
          int nLocaleFacesJ = _nFacefElement[jelem];

          for (size_t jface = 0; jface < nLocaleFacesJ; jface++) {
            int nNodeForFaceJ = _nNodefFace[jelem][jface];
            if (nLocalNodefFaceI == nNodeForFaceJ) {
              int count = 0;
              std::vector<int> nodeIndice = GetNodeIndices(jface, jelem, nLocaleFacesJ, VTK[jelem]);
              for (size_t JlocalNode = 0; JlocalNode < nNodeForFaceJ; JlocalNode++) {
                int pointIndex = startJ + nodeIndice[JlocalNode];

                count += lpoint[_element2node[pointIndex]];
                if (count == nLocalNodefFaceI) {
                  _element2element[_element2elementStart[ielem]+ iface] = jelem;

                  if (_element2element[_element2elementStart[jelem] + jface] == ielem) {
                    _element2face[_element2elementStart[ielem] + iface] = _element2face[_element2elementStart[jelem] + jface];
                  }
                  else {
                    face2nodeCount -= count;
                    _element2face[_element2elementStart[ielem] + iface] = faceCount;
                    _element2face[_element2elementStart[jelem] + jface] = faceCount;
                    _face2element[2*faceCount] = ielem;
                    _face2element[2*faceCount +1] = jelem;
                    for (size_t i = 0; i < nLocalNodefFaceI; i++) {
                      nodeCount +=1;
                    }
                    faceCount++;
                  }
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

  // Complete element2element with the ghost cells
  int elemCount = _nElem;
  for (int i = 0; i < _element2element.size(); ++i) {
		if (_element2element[i] == -1) {
			_element2element[i] = elemCount;
			elemCount += 1;
		}
	}

  // Complete element2face with the ghost cells
  for (int i = 0; i < _element2face.size(); ++i) {
		if (_element2face[i] == -1) {
			_element2face[i] = faceCount;
			faceCount += 1;
		}
	}

  printf("face2nodeCount: %2d\n",face2nodeCount );

  //compute face2element
  elemCount = _nElem;
  for (size_t ielem = 0; ielem < _nElem; ielem++) {
    int startI = _element2faceStart[ielem];
    int endI = _element2faceStart[ielem+1];
    for (size_t j = startI; j < endI; j++) {
      int iface = _element2face[j]*2;
      if (_face2element[iface] == -1) {
        _face2element[iface] = ielem;
        _face2element[iface+1] = elemCount;
        elemCount++;
      }
    }
  }

  // -------------- computing face2node -----------------------

  _face2nodeStart.resize(_nFace);
  _face2node.resize(face2nodeCount, -1);




}

const std::vector<int> Connectivity::GetNodeIndices(int face, int jelem, int nLocalFacefElement, int VTK){
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
      //printf("%d ", indiceNodes[i-start]);
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
      //printf("%d ", indiceNodes[i-start]);
    }

  } else if (VTKjelem == 13) {
    int lhelp_indice[18] = {0,1,2,0,2,5,3,0,3,4,1,1,4,5,2,3,5,4};
    int lhelp_indiceStart[nLocalFacefElement+1] = {0,3,7,11,15,18};
    int start = lhelp_indiceStart[face];
    int end = lhelp_indiceStart[face+1];
    indiceNodes.resize(end-start);

    for (size_t i = start; i < end; i++) {
      indiceNodes[i-start] = lhelp_indice[i];
      //printf("%d ", indiceNodes[i-start]);
  }
  } else if (VTKjelem == 14) {

    int lhelp_indice[16] = {0,3,2,1,0,1,4,1,2,4,2,3,4,3,0,4};

    int lhelp_indiceStart[nLocalFacefElement+1] = {0,4,7,10,13,16};

    int start = lhelp_indiceStart[face];
    int end = lhelp_indiceStart[face+1];
    indiceNodes.resize(end-start);

    for (size_t i = start; i < end; i++) {
      indiceNodes[i-start] = lhelp_indice[i];
      //printf("%d ", indiceNodes[i-start]);
    }

  }
  return indiceNodes;
}

void Connectivity::ComputeVTKLinkedLists(const std::vector<int> &VTK){


  int _nElem = VTK.size();
  _nFacefElement.resize(_nElem);
  _nNodefFace.resize(_nElem);

  for (size_t i = 0; i < _nElem; i++) {
    if (VTK[i] == 10) {
      _nFacefElement[i] = 4;
      _nNodefFace[i].resize(_nFacefElement[i]);
      _nNodefFace[i][0] = 3;
      _nNodefFace[i][1] = 3;
      _nNodefFace[i][2] = 3;
      _nNodefFace[i][3] = 3;

    }
    else if (VTK[i] == 12) {
      _nFacefElement[i] = 6;
      _nNodefFace[i].resize(_nFacefElement[i]);
      _nNodefFace[i][0] = 4;
      _nNodefFace[i][1] = 4;
      _nNodefFace[i][2] = 4;
      _nNodefFace[i][3] = 4;
      _nNodefFace[i][4] = 4;
      _nNodefFace[i][5] = 4;

    }
    else if (VTK[i] == 13) {
      _nFacefElement[i] = 5;
      _nNodefFace[i].resize(_nFacefElement[i]);
      _nNodefFace[i][0] = 3;
      _nNodefFace[i][1] = 4;
      _nNodefFace[i][2] = 4;
      _nNodefFace[i][3] = 4;
      _nNodefFace[i][4] = 3;
    }
    else if (VTK[i] == 14) {
      _nFacefElement[i] = 5;
      _nNodefFace[i].resize(_nFacefElement[i]);
      _nNodefFace[i][0] = 4;
      _nNodefFace[i][1] = 3;
      _nNodefFace[i][2] = 3;
      _nNodefFace[i][3] = 3;
      _nNodefFace[i][4] = 3;
    }
    else {
      std::cout << "vtk id not recognised" << '\n';
    }
  }
}
