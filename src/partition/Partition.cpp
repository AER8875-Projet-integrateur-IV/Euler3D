#include "partition/Partition.hpp"
#include "partition/SU2Writer.hpp"
#include "spdlog/logger.h"
#include "spdlog/stopwatch.h"
#include "utils/Logger.hpp"
#include "utils/ProgressBar.hpp"
#include <algorithm>
#include <iomanip>
#include <metis.h>
#include <regex>
#include <utility>


using namespace E3D::Partition;

void SU2Mesh::AddMarkerElement(const std::string &tag, int VTKid, int *elem2Node, int nNode) {
	// Create element vector
	std::vector<int> nodes(elem2Node, elem2Node + nNode);
	E3D::Parser::Element elem(VTKid, nodes);

	if (Markers.count(tag) == 0) {
		// No match was found in existing markers
		// Create new marker
		std::vector<E3D::Parser::Element> newElemVector;
		newElemVector.push_back(elem);
		Markers[tag] = newElemVector;
	} else {
		// The border condition exists
		// Add the new element to this condition
		Markers[tag].push_back(elem);
	}
}

void SU2Mesh::SetLocal2GlobalConnectivy(const std::vector<int> &localNode2Global,
                                        const std::vector<int> &localNode2GlobalStart) {
	int startID = localNode2GlobalStart[this->ID];
	_localNode2globalPtr = localNode2Global.data() + startID;
}


void SU2Mesh::FindPhysicalBorder(const mesh_type &mesh,
                                 const std::vector<int> &Part2ElemStart,
                                 const std::vector<int> &Part2Elem) {
	int nGlobElem = mesh.GetInteriorElementVector().size();
	physicalBorderElements.reserve(NELEM);
	// Parcours de chaque elements de la partition
	int debutE = Part2ElemStart[ID];
	int finE = Part2ElemStart[ID + 1];
	for (int iElemLoc = 0; iElemLoc < finE - debutE; iElemLoc++) {
		int iElemGlob = Part2Elem[debutE + iElemLoc];
		// Parcours des voisins de iElemGlob
		int size;
		int *elem2elem = mesh.GetElement2ElementID(iElemGlob, size);
		for (int elemGlobj = 0; elemGlobj < size; elemGlobj++) {
			// Récupération de l'élément voisin
			int jElemGlob = elem2elem[elemGlobj];
			if (jElemGlob > nGlobElem)// Element frontiere du maillage global
			{
				physicalBorderElements.push_back(iElemLoc);
				break;
			}
		}
	}
	physicalBorderElements.shrink_to_fit();
}


Partition::Partition(Mesh<E3D::Parser::SU2MeshParser> *meshGlobal, int &nPart) {
	_m_meshGlobal = meshGlobal;
	_m_nPart = nPart;
}

void Partition::solveElem2Node() {
	int NELEM = _m_meshGlobal->GetMeshInteriorElemCount();

	// Connectivité elem2node du mmaillage global
	_m_elem2NodeStart.reserve(NELEM + 1);
	_m_elem2Node.reserve(4 * NELEM);
	_m_elem2NodeStart.push_back(0);
	std::vector<E3D::Parser::Element> elems = _m_meshGlobal->GetInteriorElementVector();
	for (int iElem = 0; iElem < NELEM; iElem++) {
		_m_elem2NodeStart.push_back(_m_elem2NodeStart.back() + elems[iElem].getElemNodes().size());
	}
	_m_elem2Node.reserve(_m_elem2NodeStart.back() - 1);
	for (int iElem = 0; iElem < NELEM; iElem++) {
		for (auto node : elems[iElem].getElemNodes()) {
			_m_elem2Node.push_back(node);
		}
	}
}

void Partition::RunMetis() {
	// Initialisation
	idx_t NELEM = _m_meshGlobal->GetMeshInteriorElemCount();
	idx_t NPOIN = _m_meshGlobal->GetMeshNodeCount();
	idx_t npart(_m_nPart);
	_m_elem2Part.resize(NELEM);

	// Paramètres utiles pour appeler METIS
	std::vector<idx_t> node2Part(NPOIN);// vecteur qui va contenir la partition de chaque noeud
	idx_t objval;
	idx_t ncommon;
	if (_m_meshGlobal->GetMeshDim() == 2) {
		ncommon = 2;
	} else if (_m_meshGlobal->GetMeshDim() == 3) {
		ncommon = 3;
	}

	// Appel de METIS
	int success = METIS_PartMeshDual(&NELEM, &NPOIN, _m_elem2NodeStart.data(),
	                                 _m_elem2Node.data(), nullptr, nullptr,
	                                 &ncommon, &npart, nullptr, nullptr, &objval,
	                                 _m_elem2Part.data(), node2Part.data());

	std::cout << std::setw(30)
	          << "METIS partition success : "
	          << std::setw(6)
	          << success
	          << "\n";
	return;
}

void Partition::SolvePart2Elem() {
	int NELEM = _m_meshGlobal->GetMeshInteriorElemCount();
	// Calcul du nombre d'éléments par partition
	_m_nElemPerPart.assign(_m_nPart, 0);
	for (int iElem = 0; iElem < NELEM; iElem++) {
		_m_nElemPerPart[_m_elem2Part[iElem]]++;
	}
	// Construction de la connectivité partition vers éléments
	// // Initialisation
	_m_Part2ElemStart.reserve(_m_nPart + 1);
	_m_Part2Elem.resize(NELEM);
	_m_Part2ElemStart.push_back(0);
	// // Calcul du vecteur d'indexation
	for (int iPart = 0; iPart < _m_nPart; iPart++) {
		_m_Part2ElemStart.push_back(_m_nElemPerPart[iPart] + _m_Part2ElemStart.back());
	}
	// // Calcul du vecteur de connectivité
	for (int iElem = 0; iElem < NELEM; iElem++) {
		int part = _m_elem2Part[iElem];
		_m_Part2Elem[_m_Part2ElemStart[part]] = iElem;
		_m_Part2ElemStart[part]++;
	}
	// // Réorganisation du vecteur d'indexation
	for (int iPart = _m_nPart; iPart > 0; iPart--) {
		_m_Part2ElemStart[iPart] = _m_Part2ElemStart[iPart - 1];
	}
	_m_Part2ElemStart[0] = 0;
	return;
}

void Partition::SolveElem2Node() {
	// Initialisation
	int NELEM = _m_meshGlobal->GetMeshInteriorElemCount();
	_m_localNode2GlobalStart.reserve(_m_nPart + 1);
	_m_localNode2Global.reserve(_m_meshGlobal->GetMeshNodeCount());
	_m_localNode2GlobalStart.push_back(0);
	_m_nNodePerPart.reserve(_m_nPart);
	_m_globalElem2Local.assign(NELEM, 0);
	_m_globalNode2Local.resize(_m_nPart * _m_meshGlobal->GetMeshNodeCount());
	// Constructuction de la connectivité element vers noeuds de chaque partition
	for (int iPart = 0; iPart < _m_nPart; iPart++) {
		// Initialisation du maillage de la partition
		SU2Mesh iMesh;
		iMesh.NDIM = _m_meshGlobal->GetMeshDim();
		iMesh.NELEM = _m_nElemPerPart[iPart];
		iMesh.elem2nodeStart.reserve(iMesh.NELEM + 1);
		iMesh.elem2node.reserve(4 * _m_nElemPerPart[iPart]);//nbre de noeuds per elem>=4 (tetraedres)
		iMesh.elem2nodeStart.push_back(0);
		// Parcours de chaque elements de la partition et renumérotation des noeuds
		_m_localNode2GlobalStart.push_back(_m_localNode2GlobalStart.back());
		int debutE = _m_Part2ElemStart[iPart];
		int finE = _m_Part2ElemStart[iPart + 1];
		for (int iElemLoc = 0; iElemLoc < finE - debutE; iElemLoc++) {
			int iElemGlob = _m_Part2Elem[debutE + iElemLoc];
			_m_globalElem2Local[iElemGlob] = iElemLoc;
			// Parcours des noeuds de l'élément
			int debutN = _m_elem2NodeStart[iElemGlob];
			int finN = _m_elem2NodeStart[iElemGlob + 1];
			for (int iNode = debutN; iNode < finN; iNode++) {
				// Récupération du noeud global de l'élément
				int nodeGlob = _m_elem2Node[iNode];
				// Identification du noeud local
				int nodeLoc = Global2Local(iPart, nodeGlob);
				_m_globalNode2Local[iPart * _m_meshGlobal->GetMeshNodeCount() + nodeGlob] = nodeLoc;
				// Ajout du noeud local à la connectivité du maillage de la partition
				iMesh.elem2node.push_back(nodeLoc);
			}
			// Mis à jour de l'indexation
			iMesh.elem2nodeStart.push_back(finN - debutN + iMesh.elem2nodeStart.back());
		}
		// Calcul du nombre de noeuds de la partition
		_m_nNodePerPart.push_back(_m_localNode2GlobalStart[iPart + 1] - _m_localNode2GlobalStart[iPart]);
		// Enregistrement du maillage de la partition
		iMesh.NPOIN = _m_nNodePerPart[iPart];
		iMesh.ID = iPart;
		_m_part.push_back(iMesh);
	}

	return;
}

int Partition::Global2Local(int &iPart, int &nodeGlobal) {
	int nLocalNode = _m_localNode2GlobalStart[iPart + 1] - _m_localNode2GlobalStart[iPart];
	// Parcourir les noeuds locaux déjà affectés pour retrouver le noeud global nodeGlobal
	for (int i = 0; i < nLocalNode; i++) {
		if (nodeGlobal == _m_localNode2Global[i + _m_localNode2GlobalStart[iPart]]) {
			return i;
		}
	}
	// Créer un nouveau noeuds local pour le noeud global nodeGlobal
	_m_localNode2Global.push_back(nodeGlobal);
	_m_localNode2GlobalStart[iPart + 1]++;
	return nLocalNode;
}

int Partition::globalNode2Local(int GlobNode, int iPart) {
	return _m_globalNode2Local[_m_meshGlobal->GetMeshNodeCount() * iPart + GlobNode];
}

void Partition::SolveBorder() {
	// Initialsation
	int NELEM = _m_meshGlobal->GetMeshInteriorElemCount();
	for (int iPart = 0; iPart < _m_nPart; iPart++) {
		_m_part[iPart].Ninterface.assign(_m_nPart, 0);
		_m_part[iPart].Ninterface_elem.assign(_m_nPart, 0);
		_m_part[iPart].interface_elem.resize(_m_nPart);
		_m_part[iPart].interface_elemStart.resize(_m_nPart);
		for (int jPart = 0; jPart < _m_nPart; jPart++) {
			_m_part[iPart].interface_elemStart[jPart].push_back(0);
		}
	}
	// Calcul des interfaces entre les partitions
	for (int iPart = 0; iPart < _m_nPart; iPart++) {
		// Parcours de chaque elements de la partition
		int debutE = _m_Part2ElemStart[iPart];
		int finE = _m_Part2ElemStart[iPart + 1];
		for (int iElemLoc = 0; iElemLoc < finE - debutE; iElemLoc++) {
			int iElemGlob = _m_Part2Elem[debutE + iElemLoc];
			// Parcours des voisins de iElemGlob
			int size;
			int *elem2elem = _m_meshGlobal->GetElement2ElementID(iElemGlob, size);
			// int debutV = _m_meshGlobal->GetElement2ElementStart()[iElemGlob];
			// int finV = _m_meshGlobal->GetElement2ElementStart()[iElemGlob + 1];
			for (int elemGlobj = 0; elemGlobj < size; elemGlobj++) {
				// Récupération de l'élément voisin
				int jElemGlob = elem2elem[elemGlobj];
				if (jElemGlob < NELEM)// Element interne du maillage global
				{
					// Vérifier si l'élément est dans la partition
					int jPart = _m_elem2Part[jElemGlob];
					if (jPart > iPart)// Interface entre deux partitions
					{
						_m_part[iPart].Ninterface[jPart] = 1;
						_m_part[jPart].Ninterface[iPart] = 1;

						_m_part[iPart].Ninterface_elem[jPart]++;
						_m_part[jPart].Ninterface_elem[iPart]++;

						_m_part[iPart].interface_elem[jPart].push_back(iElemLoc);
						_m_part[iPart].interface_elem[jPart].push_back(_m_globalElem2Local[jElemGlob]);

						_m_part[jPart].interface_elem[iPart].push_back(_m_globalElem2Local[jElemGlob]);
						_m_part[jPart].interface_elem[iPart].push_back(iElemLoc);

						// Face frontière
						int nFaces;
						int idVTK(7);// Polygon
						int *faces = _m_meshGlobal->GetElement2FaceID(iElemGlob, nFaces);
						for (int iFace = 0; iFace < nFaces; iFace++) {
							int *elem = _m_meshGlobal->GetFace2ElementID(faces[iFace]);
							if ((elem[0] == jElemGlob) || (elem[1] == jElemGlob)) {
								int nNode;
								int *nodes = _m_meshGlobal->GetFace2NodeID(faces[iFace], nNode);
								_m_part[iPart].interface_elemStart[jPart].push_back(_m_part[iPart].interface_elemStart[jPart].back() + 3 + nNode);
								_m_part[jPart].interface_elemStart[iPart].push_back(_m_part[jPart].interface_elemStart[iPart].back() + 3 + nNode);
								if (nNode == 4) {// Quad
									idVTK = 9;
								} else if (nNode == 3) {// Triangle
									idVTK = 5;
								}
								_m_part[iPart].interface_elem[jPart].push_back(idVTK);
								_m_part[jPart].interface_elem[iPart].push_back(idVTK);
								for (int iNode = 0; iNode < nNode; iNode++) {

									_m_part[iPart].interface_elem[jPart].push_back(globalNode2Local(nodes[iNode], iPart));
									_m_part[jPart].interface_elem[iPart].push_back(globalNode2Local(nodes[iNode], jPart));
								}
								break;
							}
						}
					}
				} else// Condition Limite du maillage global
				{
					continue;
				}
			}
		}
	}
	return;
}

void Partition::PhysicalPartitionSolve() {
	auto &mesh = _m_meshGlobal;
	auto isInterior = [&mesh](int id) {
		int n = mesh->GetInteriorElementVector().size();
		return id < n;
	};

	int nMarker = _m_meshGlobal->GetMeshBoundaryElemCount();
	ProgressBar progress("Physical marker partitionning", nMarker, "physical marker");

	for (auto const &Marker : _m_meshGlobal->GetBoundaryConditionVector()) {
		const std::string &tag = Marker.first;
		const std::vector<E3D::Parser::Element> &elemVector = Marker.second;


		// Find a match for each border element of the global mesh
		for (auto const &elem : elemVector) {
			const std::vector<int> &markerNodes = elem.getElemNodes();
			int nMarkerNode = markerNodes.size();

			// Find global element ID of the interior element
			auto node = markerNodes.begin();
			int nElem;
			int *node2elem = _m_meshGlobal->GetNode2ElementID(*node, nElem);
			std::vector<int> commonElem(node2elem, node2elem + nElem);
			auto commonBeg = commonElem.begin();
			auto commonEnd = commonElem.end();
			std::sort(commonBeg, commonEnd);

			for (int i = 1; i < (nMarkerNode); i++) {
				node++;
				node2elem = _m_meshGlobal->GetNode2ElementID(*node, nElem);
				std::sort(node2elem, node2elem + nElem);
				// Check for matches
				std::set_intersection(commonBeg, commonEnd,
				                      node2elem, node2elem + nElem,
				                      commonBeg);
			}
			int InteriorElement = *std::find_if(commonBeg, commonEnd, isInterior);
			int partID = _m_elem2Part[InteriorElement];

			std::vector<int> localMarkerNodes;
			localMarkerNodes.reserve(nMarkerNode);
			for (int globalNode : markerNodes) {
				localMarkerNodes.push_back(globalNode2Local(globalNode, partID));
			}
			int VTKid = elem.getVtkID();
			_m_part[partID].AddMarkerElement(tag, VTKid, localMarkerNodes.data(), localMarkerNodes.size());
			progress += 1;
		}
	}
}

std::vector<E3D::Partition::SU2Mesh> &Partition::Write(
        const std::vector<std::string> &SU2OuputPath,
        std::vector<int> elem2Part) {
	auto logger = E3D::Logger::Getspdlog();
	std::cout << std::string(24, '#') << "  Partitionning  " << std::string(24, '#') << "\n"
	          << std::endl;

	std::cout << std::setw(30)
	          << "Number of Partitions : "
	          << std::setw(6)
	          << _m_nPart
	          << "\n";

	solveElem2Node();
	// skip METIS if using only one partition
	if (_m_nPart == 1) {
		elem2Part = std::vector<int>(_m_meshGlobal->GetInteriorElementVector().size(), 0);
	}

	if (elem2Part.empty()) {
		spdlog::stopwatch METISsw;
		RunMetis();
		logger->debug("METIS and set-up run time {}", METISsw);
	} else {
		int nElem = elem2Part.size();
		_m_elem2Part.resize(nElem);
		std::copy_n(elem2Part.begin(), nElem, _m_elem2Part.begin());
	}

	spdlog::stopwatch connecsw;
	SolvePart2Elem();
	SolveElem2Node();
	logger->debug("Partition connectivity run time {}", connecsw);

	spdlog::stopwatch internalMarkersw;
	SolveBorder();
	logger->debug("Internal markers partitionning run time {}", internalMarkersw);

	for (auto &part : _m_part) {
		// initial set up before resolving physical markers
		part.SetLocal2GlobalConnectivy(_m_localNode2Global, _m_localNode2GlobalStart);
	}

	spdlog::stopwatch physicalMarkersw;
	PhysicalPartitionSolve();

	logger->debug("Physical markers partitionning run time {}", physicalMarkersw);

	for (int i = 0; i < _m_nPart; i++) {
		SU2Mesh &part = _m_part[i];
		std::string path = SU2OuputPath[i];
		this->WriteSU2(part, path);
	}
	return _m_part;
}

void Partition::WriteSU2(E3D::Partition::SU2Mesh &partition, const std::string &path) {
	// vector Node
	std::vector<E3D::Parser::Node> const &globNodeVector = _m_meshGlobal->GetNodeVector();

	std::vector<E3D::Parser::Node> nodeVector;
	nodeVector.reserve(partition.NPOIN);

	for (int i = 0; i < partition.NPOIN; i++) {
		int nodeGlobI = partition.LocalNode2global(i);
		nodeVector.push_back(globNodeVector[nodeGlobI]);
	}

	// Element to Node
	std::vector<E3D::Parser::Element> elemVector;
	elemVector.reserve(partition.NELEM);

	for (int i = 0; i < partition.NELEM; i++) {
		std::vector<int> vectorNodeID;
		int nNode = partition.elem2nodeStart[i + 1] - partition.elem2nodeStart[i];
		vectorNodeID.reserve(nNode);

		for (int nodeI = partition.elem2nodeStart[i]; nodeI < partition.elem2nodeStart[i + 1]; nodeI++) {
			vectorNodeID.push_back(partition.elem2node[nodeI]);
		}

		// Find VTK id
		int start = _m_Part2ElemStart[partition.ID];
		int globElemID = _m_Part2Elem[start + i];
		int VTK = _m_meshGlobal->GetInteriorElement(globElemID).getVtkID();
		// TODO change VTK ID from 0 to good value
		elemVector.emplace_back(E3D::Parser::Element(VTK, vectorNodeID));
	}

	// Physical boundary conditions
	E3D::Parser::BC_Structure bc;
	for (const auto &marker : partition.Markers) {
		bc.push_back(marker);
	}

	// Internal Boundary
	int nAdjPart = 0;
	for (int iPart = 0; iPart < _m_nPart; iPart++) {
		nAdjPart += partition.Ninterface[iPart];
	}

	SU2Writer writer(path);
	writer.Write(elemVector, partition.NDIM, nodeVector, bc, nAdjPart, partition.Ninterface_elem, partition.interface_elem, partition.interface_elemStart);
}

void Partition::WriteTecplot(const std::string &fileName) {
	// Création du fichier
	FILE *fid = fopen(fileName.c_str(), "w");
	// Cas d'un maillage 2D
	if (_m_meshGlobal->GetMeshDim() == 2) {
		// Entête du fichier
		fprintf(fid, "VARIABLES=\"X\",\"Y\"\n");
		// Écriture de chaque partition dans une zone de Tecplot
		for (int iPart = 0; iPart < _m_nPart; iPart++) {
			// Entête de la zone
			int nNodes = _m_nNodePerPart[iPart];
			int nElements = _m_nElemPerPart[iPart];
			fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEPOLYGON\nDATAPACKING=POINT\n", nNodes, nElements);

			// Coordonnées des noeuds de la partition
			for (int nodeI = 0; nodeI < nNodes; nodeI++) {
				E3D::Parser::Node node = _m_meshGlobal->GetNodeCoord(_m_localNode2Global[nodeI + _m_localNode2GlobalStart[iPart]]);
				fprintf(fid, "%.12e %.12e\n", node.getX(), node.getY());
			}

			// Connectivité des éléments de la partition
			for (int elementI = 0; elementI < nElements; elementI++) {
				for (int iNode = _m_part[iPart].elem2nodeStart[elementI]; iNode < _m_part[iPart].elem2nodeStart[elementI + 1]; iNode++) {
					fprintf(fid, "%d ", _m_part[iPart].elem2node[iNode] + 1);
				}
				fprintf(fid, "\n");
			}
		}
	}
	// Cas d'un maillage 3D
	// Elements ayant seulement 8 noeuds seront considérés
	else if (_m_meshGlobal->GetMeshDim() == 3) {
		// Entête du fichier
		fprintf(fid, "VARIABLES=\"X\",\"Y\",\"Z\"\n");
		for (int iPart = 0; iPart < _m_nPart; iPart++) {
			// Entête de la zone
			int nNodes = _m_nNodePerPart[iPart];
			int nElements = _m_nElemPerPart[iPart];
			fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEBRICK\nDATAPACKING=POINT\n", nNodes, nElements);

			// Coordonnées des noeuds de la partition
			for (int nodeI = 0; nodeI < nNodes; nodeI++) {
				E3D::Parser::Node node = _m_meshGlobal->GetNodeCoord(_m_localNode2Global[nodeI + _m_localNode2GlobalStart[iPart]]);
				fprintf(fid, "%.12e %.12e %.12e\n", node.getX(), node.getY(), node.getZ());
			}

			// Connectivité des éléments de la partition
			for (int elementI = 0; elementI < nElements; elementI++) {
				for (int iNode = _m_part[iPart].elem2nodeStart[elementI]; iNode < _m_part[iPart].elem2nodeStart[elementI + 1]; iNode++) {
					fprintf(fid, "%d ", _m_part[iPart].elem2node[iNode] + 1);
				}
				fprintf(fid, "\n");
			}
		}
	}
	// Fermeture du fichier
	fclose(fid);
	return;
}
