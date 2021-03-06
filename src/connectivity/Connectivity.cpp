#include "connectivity/Connectivity.hpp"
#include <stdlib.h>


void Connectivity::SolveElement2node(const std::vector<E3D::Parser::Element> &elemVector) {

	_nElem = elemVector.size();
	_element2nodeStart.resize(_nElem + 1, 0);
	int counter = 0;
	for (int ielem = 0; ielem < _nElem; ielem++) {
		E3D::Parser::Element elem = elemVector[ielem];
		counter += elem.getElemNodes().size();
		_element2nodeStart[ielem + 1] = counter;
	}
	_element2node.resize(counter);

	//construire element2nodeStart
	for (int ielem = 0; ielem < _nElem; ielem++) {
		int localindex = 0;
		for (int i = _element2nodeStart[ielem]; i < _element2nodeStart[ielem + 1]; i++) {

			_element2node[i] = elemVector[ielem].getElemNodes()[localindex];
			localindex++;
		}
	}
}

void Connectivity::SolveNode2element() {

	//node2element
	_node2elementStart.resize(_nNode + 1);
	_node2element.resize(_element2nodeStart[_nElem]);

	int startI;
	int endI;
	int nodeI;
	for (int ielem = 0; ielem < _nElem; ielem++) {


		startI = _element2nodeStart[ielem];
		endI = _element2nodeStart[ielem + 1];

		for (int i = startI; i < endI; i++) {
			nodeI = _element2node[i];
			_node2elementStart[nodeI + 1] += 1;
		}
	}
	for (size_t i = 1; i < _node2elementStart.size(); i++) {
		_node2elementStart[i] += _node2elementStart[i - 1];
		//printf("%2d ", _node2elementStart[i] );
	}

	int j;
	std::vector<int> store;
	store.resize(_nNode);
	//int store[_nNode] = {0};
	for (int ielem = 0; ielem < _nElem; ielem++) {
		startI = _element2nodeStart[ielem];
		endI = _element2nodeStart[ielem + 1];

		for (int i = startI; i < endI; i++) {
			nodeI = _element2node[i];
			j = _node2elementStart[nodeI] + store[nodeI];
			_node2element[j] = ielem;
			store[nodeI]++;
		}
	}
}

void Connectivity::SolveElement2Element(const std::vector<int> &VTK, int BoundaryFaces) {


	_element2faceStart.resize(_nElem + 1);
	_element2elementStart.resize(_nElem + 1);

	for (int i = 1; i < _nElem + 1; i++) {
		_element2elementStart[i] = _nFacefElement[i - 1];
		_element2elementStart[i] += _element2elementStart[i - 1];
		_element2faceStart[i] = _element2elementStart[i];
	}


	_nFace = (BoundaryFaces + _element2elementStart[_nElem]) / 2;
	_nFaceInt = _nFace - BoundaryFaces;


	_face2element.resize(_nFace * 2, -1);
	_element2face.resize(_element2elementStart[_nElem], -1);
	_element2element.resize(_element2elementStart[_nElem], -1);
	_face2nodeStart.resize(_nFace + 1, 0);
	_face2node.resize(_nFace * 4, -1);


	//creating lhelp_indice vector
	// std::vector<std::vector<int>> _lhelp_indice{{0, 2, 1, 0, 1, 3, 1, 2, 3, 2, 0, 3},
	//                                             {0, 3, 2, 1, 0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6, 3, 0, 4, 7, 4, 5, 6, 7},
	//                                             {0, 1, 2, 0, 2, 5, 3, 0, 3, 4, 1, 1, 4, 5, 2, 3, 5, 4},
	//                                             {0, 3, 2, 1, 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4}};

	// std::vector<std::vector<int>> _lhelp_indiceStart{{0, 3, 6, 9,  12},
	//                                                  {0, 4, 8, 12, 16, 20, 24},
	//                                                  {0, 3, 7, 11, 15, 18},
	//                                                  {0, 4, 7, 10, 13, 16}};


	int nLocalFacefElement;
	int faceCount = 0;
	int face2nodeCount = 0;

	for (int ielem = 0; ielem < _nElem; ielem++) {
		int startI = _element2nodeStart[ielem];
		//int endI = _element2nodeStart[ielem + 1];
		int ilhelp;


		// ---------- determine le type d'element ---------------
		if (VTK[ielem] == 10) {
			//methode pour tetrahedre
			nLocalFacefElement = _nFacefElement[ielem];
			face2nodeCount += 12;
			ilhelp = 0;

		} else if (VTK[ielem] == 12) {
			//methode pour cube
			nLocalFacefElement = _nFacefElement[ielem];
			face2nodeCount += 24;
			ilhelp = 1;

		} else if (VTK[ielem] == 13) {
			//methode pour wedge
			nLocalFacefElement = _nFacefElement[ielem];
			face2nodeCount += 18;
			ilhelp = 2;

		} else if (VTK[ielem] == 14) {
			//methode pour pyramide base carree
			nLocalFacefElement = _nFacefElement[ielem];
			face2nodeCount += 16;
			ilhelp = 3;

		} else {
			std::cout << "VTK not recognised!" << '\n';
			nLocalFacefElement = -1;
			ilhelp = -1;
			exit(EXIT_FAILURE);
		}


		// ---------- commence la connectivity element2element et face ---------------
		for (int iface = 0; iface < nLocalFacefElement; iface++) {
			int nLocalNodefFaceI = _nNodefFace[ielem][iface];
			std::vector<int> lhelp;
			lhelp.resize(nLocalNodefFaceI);
			//int lhelp[nLocalNodefFaceI] = {0};
			std::vector<int> lpoint;
			lpoint.resize(_nNode);
			//int lpoint[_nNode] = {0};
			for (int inode = 0; inode < nLocalNodefFaceI; inode++) {
				lhelp[inode] = _element2node[startI + _lhelp_indice[ilhelp][_lhelp_indiceStart[ilhelp][iface] + inode]];
				lpoint[lhelp[inode]] = 1;
			}
			int ipoint = lhelp[0];

			int elemStart = _node2elementStart[ipoint];
			int elemEnd = _node2elementStart[ipoint + 1];

			for (int j = elemStart; j < elemEnd; j++) {
				int jelem = _node2element[j];
				if (jelem != ielem) {
					int startJ = _element2nodeStart[jelem];
					//int endJ = _element2nodeStart[jelem + 1];
					int nLocaleFacesJ = _nFacefElement[jelem];

					for (int jface = 0; jface < nLocaleFacesJ; jface++) {
						int nNodeForFaceJ = _nNodefFace[jelem][jface];
						if (nLocalNodefFaceI == nNodeForFaceJ) {
							int count = 0;
							std::vector<int> nodeIndice = GetNodeIndices(jface, VTK[jelem]);
							for (int JlocalNode = 0; JlocalNode < nNodeForFaceJ; JlocalNode++) {
								int pointIndex = startJ + nodeIndice[JlocalNode];

								count += lpoint[_element2node[pointIndex]];
								if (count == nLocalNodefFaceI) {
									_element2element[_element2elementStart[ielem] + iface] = jelem;

									if (_element2element[_element2elementStart[jelem] + jface] == ielem) {
										_element2face[_element2elementStart[ielem] + iface] = _element2face[_element2elementStart[jelem] + jface];
									} else {
										face2nodeCount -= count;//voir si utile
										_element2face[_element2elementStart[ielem] + iface] = faceCount;
										_element2face[_element2elementStart[jelem] + jface] = faceCount;
										_face2element[2 * faceCount] = ielem;
										_face2element[2 * faceCount + 1] = jelem;
										_face2nodeStart[faceCount + 1] = _face2nodeStart[faceCount] + nNodeForFaceJ;
										for (int inode = 0; inode < nNodeForFaceJ; inode++) {
											_face2node[_face2nodeStart[faceCount] + inode] = lhelp[inode];
										}
										faceCount++;
									}
								}
							}
						}
					}
				}
			}
			for (int i = 0; i < nLocalNodefFaceI; i++) {
				lpoint[lhelp[i]] = 0;
			}
		}
	}

	// Complete element2element with the ghost cells
	int elemCount = _nElem;
	for (size_t i = 0; i < _element2element.size(); ++i) {
		if (_element2element[i] == -1) {
			_element2element[i] = elemCount;
			elemCount += 1;
		}
	}

	// Complete element2face with the ghost cells
	for (size_t i = 0; i < _element2face.size(); ++i) {
		if (_element2face[i] == -1) {
			_element2face[i] = faceCount;
			faceCount += 1;
		}
	}

	//printf("face2nodeCount: %2d\n",face2nodeCount );

	//compute face2element
	elemCount = _nElem;
	for (int ielem = 0; ielem < _nElem; ielem++) {
		int startI = _element2faceStart[ielem];
		int endI = _element2faceStart[ielem + 1];
		for (int j = startI; j < endI; j++) {
			int iface = _element2face[j] * 2;
			if (_face2element[iface] == -1) {
				_face2element[iface] = ielem;
				_face2element[iface + 1] = elemCount;
				elemCount++;
			}
		}
	}
	//complete facenode avec les boundary faces
	for (int iface = _nFace - BoundaryFaces; iface < _nFace; iface++) {
		int ielem = _face2element[2 * iface];
		int startI = _element2faceStart[ielem];
		int endI = _element2faceStart[ielem + 1];
		for (int j = startI; j < endI; j++) {
			if (_element2face[j] == iface) {
				std::vector<int> face2nodeIndice = GetNodeIndices(j - startI, VTK[ielem]);
				_face2nodeStart[iface + 1] = _face2nodeStart[iface] + face2nodeIndice.size();
				for (size_t inode = 0; inode < face2nodeIndice.size(); inode++) {
					_face2node[_face2nodeStart[iface] + inode] = _element2node[_element2nodeStart[ielem] +
					                                                           face2nodeIndice[inode]];
				}
			}
		}
	}
	_face2node.resize(_face2nodeStart[_nFace]);
}

const std::vector<int> Connectivity::GetNodeIndices(int face, int VTK) {
	// enlever la redondance entre le VTKjelem et jelem
	int VTKjelem = VTK;
	std::vector<int> indiceNodes;
	if (VTKjelem == 10) {
		int lhelp_indice10[12] = {0, 2, 1, 0, 1, 3, 1, 2, 3, 2, 0, 3};
		//int lhelp_indiceStart[nLocalFacefElement + 1] = {0, 3, 6, 9, 12};
		int start = _lhelp_indiceStart[0][face];
		int end = _lhelp_indiceStart[0][face + 1];
		indiceNodes.resize(end - start);
		for (int i = start; i < end; i++) {
			indiceNodes[i - start] = lhelp_indice10[i];
			//printf("%d ", indiceNodes[i-start]);
		}
	} else if (VTKjelem == 12) {
		int lhelp_indice12[24] = {0, 3, 2, 1, 0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6, 3, 0, 4, 7, 4, 5, 6, 7};
		//int lhelp_indiceStart[nLocalFacefElement + 1] = {0, 4, 8, 12, 16, 20, 24};
		int start = _lhelp_indiceStart[1][face];
		int end = _lhelp_indiceStart[1][face + 1];
		indiceNodes.resize(end - start);
		for (int i = start; i < end; i++) {
			indiceNodes[i - start] = lhelp_indice12[i];
			//printf("%d ", indiceNodes[i-start]);
		}

	} else if (VTKjelem == 13) {
		int lhelp_indice13[18] = {0, 1, 2, 0, 2, 5, 3, 0, 3, 4, 1, 1, 4, 5, 2, 3, 5, 4};
		//int lhelp_indiceStart[nLocalFacefElement + 1] = {0, 3, 7, 11, 15, 18};
		int start = _lhelp_indiceStart[2][face];
		int end = _lhelp_indiceStart[2][face + 1];
		indiceNodes.resize(end - start);

		for (int i = start; i < end; i++) {
			indiceNodes[i - start] = lhelp_indice13[i];
			//printf("%d ", indiceNodes[i-start]);
		}
	} else if (VTKjelem == 14) {

		int lhelp_indice14[16] = {0, 3, 2, 1, 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4};
		//int lhelp_indiceStart[nLocalFacefElement + 1] = {0, 4, 7, 10, 13, 16};
		int start = _lhelp_indiceStart[3][face];
		int end = _lhelp_indiceStart[3][face + 1];
		indiceNodes.resize(end - start);

		for (int i = start; i < end; i++) {
			indiceNodes[i - start] = lhelp_indice14[i];
			//printf("%d ", indiceNodes[i-start]);
		}
	}
	return indiceNodes;
}

void Connectivity::ComputeVTKLinkedLists(const std::vector<int> &VTK) {


	_nElem = VTK.size();
	_nFacefElement.resize(_nElem);
	_nNodefFace.resize(_nElem);

	for (int i = 0; i < _nElem; i++) {
		if (VTK[i] == 10) {
			_nFacefElement[i] = 4;
			_nNodefFace[i].resize(_nFacefElement[i]);
			_nNodefFace[i][0] = 3;
			_nNodefFace[i][1] = 3;
			_nNodefFace[i][2] = 3;
			_nNodefFace[i][3] = 3;

		} else if (VTK[i] == 12) {
			_nFacefElement[i] = 6;
			_nNodefFace[i].resize(_nFacefElement[i]);
			_nNodefFace[i][0] = 4;
			_nNodefFace[i][1] = 4;
			_nNodefFace[i][2] = 4;
			_nNodefFace[i][3] = 4;
			_nNodefFace[i][4] = 4;
			_nNodefFace[i][5] = 4;

		} else if (VTK[i] == 13) {
			_nFacefElement[i] = 5;
			_nNodefFace[i].resize(_nFacefElement[i]);
			_nNodefFace[i][0] = 3;
			_nNodefFace[i][1] = 4;
			_nNodefFace[i][2] = 4;
			_nNodefFace[i][3] = 4;
			_nNodefFace[i][4] = 3;
		} else if (VTK[i] == 14) {
			_nFacefElement[i] = 5;
			_nNodefFace[i].resize(_nFacefElement[i]);
			_nNodefFace[i][0] = 4;
			_nNodefFace[i][1] = 3;
			_nNodefFace[i][2] = 3;
			_nNodefFace[i][3] = 3;
			_nNodefFace[i][4] = 3;
		} else {
			std::cout << "vtk id not recognised" << '\n';
		}
	}
}
