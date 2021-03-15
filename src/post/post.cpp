/**
 * @file Post.cpp
 * @author Vincent Bar, Aziz Ndiaye, Abraham Bherer-Constant
 * @date 2021/02/01
 * @brief Write a solution file to read on Tecplot
 *
 * Wrtie a solution file. Write a mesh on which connectivity has already been solved with
 * the METIS library into multiple smaller mesh and write as well solution for other parameters
 * such as Pressure, Density, Velocity and Energy already solved in Solver.
 */
#include "post/post.hpp"
#include "TECIO.h"
#include "spdlog/logger.h"
#include "spdlog/stopwatch.h"
#include "utils/Logger.hpp"
#include <algorithm>
#include <utility>


using namespace E3D::Post;


Post::Post(const E3D::Parser::SimConfig &config) {
	_outputFile = config.getTecplotFile();
	_nPart = config.getNumberPartitions();
	_meshPartitionPath = config.getPartitionedMeshFiles();
	_solutionPartitionPath.reserve(_nPart);
	for (int i = 0; i < _nPart; i++) {
		_solutionPartitionPath.push_back(_meshPartitionPath[i] + ".sol");
	}
	return;
}

Post::~Post() {
	return;
}

void Post::Write() {
	std::cout << std::string(24, '#') << "  TECPLOT  " << std::string(24, '#') << "\n"
	          << std::endl;
	auto logger = E3D::Logger::Getspdlog();
	spdlog::stopwatch tecplotASCIIsw;
	WriteTecplotASCII();
	std::cout << "Output ASCII File: " << _outputFile << std::endl;
	logger->debug("Writing Tecplot ASCII file run time {}", tecplotASCIIsw);
	spdlog::stopwatch tecplotBinarysw;
	WriteTecplotBinary();
	logger->debug("Writing Tecplot Binary file run time {}", tecplotBinarysw);
	std::cout << "Output Binary File: " << _outputFile << std::endl;
	WriteTecplotSurfaceASCII();
	std::cout << "\n";
}

void Post::WriteTecplotASCII() {

	// Création du fichier
	FILE *fid = fopen(_outputFile.c_str(), "w");
	// Entête du fichier
	fprintf(fid, "VARIABLES=\"X\",\"Y\",\"Z\",\"Rho\",\"U\",\"V\",\"W\",\"P\",\"E\"\n");
	for (int iPart = 0; iPart < _nPart; iPart++) {
		// Lecture de la partition
		E3D::Parser::SU2MeshParser iMesh = E3D::Parser::SU2MeshParser(_meshPartitionPath[iPart]);
		// Entête de la zone
		int nNodes = iMesh.GetPointsCount();
		int nElements = iMesh.GetVolumeElemCount();
		fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEBRICK\nDATAPACKING=BLOCK, VARLOCATION=([4-9]=CELLCENTERED)\n", nNodes, nElements);

		// Coordonnées des noeuds de la partition
		for (int nodeI = 0; nodeI < nNodes; nodeI++) {
			E3D::Parser::Node node = iMesh.GetPoints()[nodeI];
			fprintf(fid, "%.12e\n", node.getX());
		}
		for (int nodeI = 0; nodeI < nNodes; nodeI++) {
			E3D::Parser::Node node = iMesh.GetPoints()[nodeI];
			fprintf(fid, "%.12e\n", node.getY());
		}
		for (int nodeI = 0; nodeI < nNodes; nodeI++) {
			E3D::Parser::Node node = iMesh.GetPoints()[nodeI];
			fprintf(fid, "%.12e\n", node.getZ());
		}

		// Lecture des solutions
		E3D::Parser::SolutionPost isolution(_solutionPartitionPath[iPart], nElements);
		for (int iElem = 0; iElem < nElements; iElem++) {
			fprintf(fid, "%.12e\n", isolution.GetRho(iElem));
		}
		for (int iElem = 0; iElem < nElements; iElem++) {
			fprintf(fid, "%.12e\n", isolution.GetU(iElem));
		}
		for (int iElem = 0; iElem < nElements; iElem++) {
			fprintf(fid, "%.12e\n", isolution.GetV(iElem));
		}
		for (int iElem = 0; iElem < nElements; iElem++) {
			fprintf(fid, "%.12e\n", isolution.GetW(iElem));
		}
		for (int iElem = 0; iElem < nElements; iElem++) {
			fprintf(fid, "%.12e\n", isolution.GetPression(iElem));
		}
		for (int iElem = 0; iElem < nElements; iElem++) {
			fprintf(fid, "%.12e\n", isolution.GetEnergy(iElem));
		}

		// Connectivité des éléments de la partition
		for (const E3D::Parser::Element &elem : iMesh.GetVolumeElems()) {
			if (elem.getVtkID() == 12)// Hexaedre
			{
				fprintf(fid, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
				        elem.getElemNodes()[0] + 1, elem.getElemNodes()[1] + 1, elem.getElemNodes()[2] + 1, elem.getElemNodes()[3] + 1,
				        elem.getElemNodes()[4] + 1, elem.getElemNodes()[5] + 1, elem.getElemNodes()[6] + 1, elem.getElemNodes()[7] + 1);
			} else if (elem.getVtkID() == 10)// Tetraedre
			{
				fprintf(fid, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
				        elem.getElemNodes()[0] + 1, elem.getElemNodes()[1] + 1, elem.getElemNodes()[2] + 1, elem.getElemNodes()[2] + 1,
				        elem.getElemNodes()[3] + 1, elem.getElemNodes()[3] + 1, elem.getElemNodes()[3] + 1, elem.getElemNodes()[3] + 1);
			} else if (elem.getVtkID() == 14)// Pyramid
			{
				fprintf(fid, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
				        elem.getElemNodes()[0] + 1, elem.getElemNodes()[1] + 1, elem.getElemNodes()[2] + 1, elem.getElemNodes()[3] + 1,
				        elem.getElemNodes()[4] + 1, elem.getElemNodes()[4] + 1, elem.getElemNodes()[4] + 1, elem.getElemNodes()[4] + 1);
			} else if (elem.getVtkID() == 13)// Prism
			{
				fprintf(fid, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
				        elem.getElemNodes()[0] + 1, elem.getElemNodes()[1] + 1, elem.getElemNodes()[1] + 1, elem.getElemNodes()[2] + 1,
				        elem.getElemNodes()[3] + 1, elem.getElemNodes()[4] + 1, elem.getElemNodes()[4] + 1, elem.getElemNodes()[5] + 1);
			} else {

				for (const int &iNode : elem.getElemNodes()) {
					fprintf(fid, "%d\t", iNode + 1);
				}
				fprintf(fid, "\n");
			}
		}
	}

	// Fermeture du fichier
	fclose(fid);
	return;
}


void Post::WriteTecplotBinary() {
	//Initialisation du fichier binaire
	char const *Title = "Titre du fichier";
	char const *Variables = "X,Y,Z,Rho,U,V,W,P,E";
	_outputFile += ".plt";
	char const *FName = _outputFile.c_str();
	char const *ScratchDir = ".";
	INTEGER4 FileFormat = 0;// 0 = .plt et 1 = .szplt
	INTEGER4 FileType = 0;  // 0=FUll (mesh+solution)
	INTEGER4 Debug = 1;     // mode debug activé
	INTEGER4 VIsDouble = 1; // precision double
	INTEGER4 I = 0;         // retour de la fonction tecplot

	I = tecini142(Title, Variables, FName, ScratchDir, &FileFormat, &FileType, &Debug, &VIsDouble);

	// Écriture des zones
	// Initialisation de la zone
	for (int iPart = 0; iPart < _nPart; iPart++) {

		E3D::Parser::SU2MeshParser iMesh = E3D::Parser::SU2MeshParser(_meshPartitionPath[iPart]);
		std::string titreZone = "Partition " + std::to_string(iPart);
		char const *ZoneTitle = titreZone.c_str();
		INTEGER4 ZoneType = 5;                                      // 5 = FEBRICK
		INTEGER4 NumPts = iMesh.GetPointsCount();                   // Nombre de noeuds de la zone
		INTEGER4 NumElements = iMesh.GetVolumeElemCount();          // Nombre d'élements
		INTEGER4 NumFaces(0), ICellMax(0), JCellMax(0), KCellMax(0);// Non utilisé
		double SolutionTime = 0.;
		INTEGER4 StrandID = 0;  // zone statique
		INTEGER4 ParentZone = 0;// pas de zone parent
		INTEGER4 IsBlock = 1;
		INTEGER4 NumFaceConnections(0), FaceNeighborMode(0), TotalNumFaceNodes(0);// Non utilisé
		INTEGER4 NumConnectedBoundaryFaces(0), TotalNumBoundaryConnections(0);    // Non utilisé
		INTEGER4 ValueLocation[] = {1, 1, 1, 0, 0, 0, 0, 0, 0};                   // 0 = cell-centered et 1 = node-centered
		INTEGER4 ShareConnectivityFromZone = 0;

		I = teczne142(ZoneTitle, &ZoneType, &NumPts, &NumElements, &NumFaces, &ICellMax, &JCellMax, &KCellMax,
		              &SolutionTime, &StrandID, &ParentZone, &IsBlock, &NumFaceConnections, &FaceNeighborMode,
		              &TotalNumFaceNodes, &NumConnectedBoundaryFaces, &TotalNumBoundaryConnections, NULL,
		              ValueLocation, NULL, &ShareConnectivityFromZone);

		// Récupération des variables
		double *X = new double[NumPts];
		double *Y = new double[NumPts];
		double *Z = new double[NumPts];
		for (int nodeI = 0; nodeI < NumPts; nodeI++) {
			E3D::Parser::Node node = iMesh.GetPoints()[nodeI];
			X[nodeI] = node.getX();
			Y[nodeI] = node.getY();
			Z[nodeI] = node.getZ();
		}
		E3D::Parser::SolutionPost isolution(_solutionPartitionPath[iPart], NumElements);

		// Écriture des variables de la zone
		I = tecdat142(&NumPts, X, &VIsDouble);
		I = tecdat142(&NumPts, Y, &VIsDouble);
		I = tecdat142(&NumPts, Z, &VIsDouble);

		I = tecdat142(&NumElements, isolution.GetRho().data(), &VIsDouble);
		I = tecdat142(&NumElements, isolution.GetU().data(), &VIsDouble);
		I = tecdat142(&NumElements, isolution.GetV().data(), &VIsDouble);
		I = tecdat142(&NumElements, isolution.GetW().data(), &VIsDouble);
		I = tecdat142(&NumElements, isolution.GetPression().data(), &VIsDouble);
		I = tecdat142(&NumElements, isolution.GetEnergy().data(), &VIsDouble);

		delete[] X;
		delete[] Y;
		delete[] Z;

		// Connectivité de la zone
		INTEGER4 N = 8 * NumElements;// FEBRICK : chaque elément a 8 noeuds
		INTEGER4 *NData = new INTEGER4[N];
		std::vector<E3D::Parser::Element> elem = iMesh.GetVolumeElems();
		for (int iElem = 0; iElem < NumElements; iElem++) {
			if (elem[iElem].getVtkID() == 12)// Hexaedre
			{
				NData[iElem * 8 + 0] = elem[iElem].getElemNodes()[0] + 1;
				NData[iElem * 8 + 1] = elem[iElem].getElemNodes()[1] + 1;
				NData[iElem * 8 + 2] = elem[iElem].getElemNodes()[2] + 1;
				NData[iElem * 8 + 3] = elem[iElem].getElemNodes()[3] + 1;
				NData[iElem * 8 + 4] = elem[iElem].getElemNodes()[4] + 1;
				NData[iElem * 8 + 5] = elem[iElem].getElemNodes()[5] + 1;
				NData[iElem * 8 + 6] = elem[iElem].getElemNodes()[6] + 1;
				NData[iElem * 8 + 7] = elem[iElem].getElemNodes()[7] + 1;

			} else if (elem[iElem].getVtkID() == 10)// Tetraedre
			{
				NData[iElem * 8 + 0] = elem[iElem].getElemNodes()[0] + 1;
				NData[iElem * 8 + 1] = elem[iElem].getElemNodes()[1] + 1;
				NData[iElem * 8 + 2] = elem[iElem].getElemNodes()[2] + 1;
				NData[iElem * 8 + 3] = elem[iElem].getElemNodes()[2] + 1;
				NData[iElem * 8 + 4] = elem[iElem].getElemNodes()[3] + 1;
				NData[iElem * 8 + 5] = elem[iElem].getElemNodes()[3] + 1;
				NData[iElem * 8 + 6] = elem[iElem].getElemNodes()[3] + 1;
				NData[iElem * 8 + 7] = elem[iElem].getElemNodes()[3] + 1;

			} else if (elem[iElem].getVtkID() == 14)// Pyramid
			{
				NData[iElem * 8 + 0] = elem[iElem].getElemNodes()[0] + 1;
				NData[iElem * 8 + 1] = elem[iElem].getElemNodes()[1] + 1;
				NData[iElem * 8 + 2] = elem[iElem].getElemNodes()[2] + 1;
				NData[iElem * 8 + 3] = elem[iElem].getElemNodes()[3] + 1;
				NData[iElem * 8 + 4] = elem[iElem].getElemNodes()[4] + 1;
				NData[iElem * 8 + 5] = elem[iElem].getElemNodes()[4] + 1;
				NData[iElem * 8 + 6] = elem[iElem].getElemNodes()[4] + 1;
				NData[iElem * 8 + 7] = elem[iElem].getElemNodes()[4] + 1;

			} else if (elem[iElem].getVtkID() == 13)// Prism
			{
				NData[iElem * 8 + 0] = elem[iElem].getElemNodes()[0] + 1;
				NData[iElem * 8 + 1] = elem[iElem].getElemNodes()[1] + 1;
				NData[iElem * 8 + 2] = elem[iElem].getElemNodes()[1] + 1;
				NData[iElem * 8 + 3] = elem[iElem].getElemNodes()[2] + 1;
				NData[iElem * 8 + 4] = elem[iElem].getElemNodes()[3] + 1;
				NData[iElem * 8 + 5] = elem[iElem].getElemNodes()[4] + 1;
				NData[iElem * 8 + 6] = elem[iElem].getElemNodes()[4] + 1;
				NData[iElem * 8 + 7] = elem[iElem].getElemNodes()[5] + 1;

			} else {
				std::cout << "Unrecognised VTK Id\n";
			}
		}

		I = tecnode142(&N, NData);
		delete[] NData;
	}

	// Fermeture du fichier binaire
	I = tecend142();
	return;
}


void Post::WriteTecplotSurfaceASCII() {

	// Création du fichier
	std::string fileName = _outputFile + ".surf.dat";
	FILE *fid = fopen(fileName.c_str(), "w");
	// Entête du fichier
	fprintf(fid, "VARIABLES=\"X\",\"Y\",\"Z\",\"Rho\",\"U\",\"V\",\"W\",\"P\",\"E\",\"Cp\"\n");
	for (int iPart = 0; iPart < _nPart; iPart++) {
		// Lecture de la partition
		//E3D::Parser::SU2MeshParser iMesh = E3D::Parser::SU2MeshParser(_meshPartitionPath[iPart]);
		E3D::Mesh<E3D::Parser::SU2MeshParser> iMesh(_meshPartitionPath[iPart]);
		iMesh.solveConnectivity();
		std::vector<int> WallGhostCellIDs;
		std::vector<int> WallAdjacentToGhostCellIDs;
		std::vector<int> WallAdjacentFaceIDs;
		std::vector<int> facesAroundGhostCells;

		int interiorElemsCount = iMesh.GetMeshInteriorElemCount();
		for (int faceID = 0; faceID < iMesh.GetnFace(); faceID++) {
			int *p_face2elem = iMesh.GetFace2ElementID(faceID);
			if (p_face2elem[1] >= interiorElemsCount) {
				facesAroundGhostCells.push_back(faceID);
			}
		}
		for (auto &[partitionTag, faces] : iMesh.GetBoundaryConditionVector()) {
			auto Tag = partitionTag;
			// Transform it to be case insensitive
			std::transform(Tag.begin(), Tag.end(), Tag.begin(), ::tolower);
			if (Tag == "airfoil" || Tag == "wall") {
				WallGhostCellIDs.reserve(faces.size());
				WallAdjacentToGhostCellIDs.reserve(faces.size());
				WallAdjacentFaceIDs.reserve(faces.size());
				for (auto face : faces) {

					auto wallFaceNodes = face.getElemNodes();
					std::sort(wallFaceNodes.begin(), wallFaceNodes.end());
					for (auto &faceConnectedToGC : facesAroundGhostCells) {
						int numNodes = 0;
						int *p_faceToNodes = iMesh.GetFace2NodeID(faceConnectedToGC, numNodes);
						if (numNodes > 0) {
							std::vector<int> tempNodes;
							// std::cout << "NNodes: " << numNodes << "Face GC id: " << faceConnectedToGC << std::endl;
							tempNodes.reserve(numNodes);
							for (int i = 0; i < numNodes; i++) {
								tempNodes.push_back(p_faceToNodes[i]);
							}
							std::sort(tempNodes.begin(), tempNodes.end());
							if (tempNodes == wallFaceNodes) {
								WallGhostCellIDs.push_back(iMesh.GetFace2ElementID(faceConnectedToGC)[1]);
								WallAdjacentToGhostCellIDs.push_back(iMesh.GetFace2ElementID(faceConnectedToGC)[0]);
								WallAdjacentFaceIDs.push_back(faceConnectedToGC);
								break;
							}
						} else {
							continue;
						}
					}
				}
			}
		}

		int nNodes = iMesh.GetMeshNodeCount();
		int nElements = WallGhostCellIDs.size();
		if (nElements != 0) {
			fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEQUADRILATERAL\nDATAPACKING=BLOCK, VARLOCATION=([4-10]=CELLCENTERED)\n", nNodes, nElements);

			// Coordonnées des noeuds de la partition
			for (int nodeI = 0; nodeI < nNodes; nodeI++) {
				const E3D::Parser::Node node = iMesh.GetNodeCoord(nodeI);
				fprintf(fid, "%.12e\n", node.getX());
			}
			for (int nodeI = 0; nodeI < nNodes; nodeI++) {
				const E3D::Parser::Node node = iMesh.GetNodeCoord(nodeI);
				fprintf(fid, "%.12e\n", node.getY());
			}
			for (int nodeI = 0; nodeI < nNodes; nodeI++) {
				const E3D::Parser::Node node = iMesh.GetNodeCoord(nodeI);
				fprintf(fid, "%.12e\n", node.getZ());
			}

			// Lecture des solutions
			E3D::Parser::SolutionPost isolution(_solutionPartitionPath[iPart], iMesh.GetMeshInteriorElemCount());

			for (size_t GhostID = 0; GhostID < WallGhostCellIDs.size(); GhostID++) {
				int InteriorGhostIdx = WallAdjacentToGhostCellIDs[GhostID];
				fprintf(fid, "%.12e\n", isolution.GetRho(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < WallGhostCellIDs.size(); GhostID++) {
				int InteriorGhostIdx = WallAdjacentToGhostCellIDs[GhostID];
				fprintf(fid, "%.12e\n", isolution.GetU(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < WallGhostCellIDs.size(); GhostID++) {
				int InteriorGhostIdx = WallAdjacentToGhostCellIDs[GhostID];
				fprintf(fid, "%.12e\n", isolution.GetV(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < WallGhostCellIDs.size(); GhostID++) {
				int InteriorGhostIdx = WallAdjacentToGhostCellIDs[GhostID];
				fprintf(fid, "%.12e\n", isolution.GetW(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < WallGhostCellIDs.size(); GhostID++) {
				int InteriorGhostIdx = WallAdjacentToGhostCellIDs[GhostID];
				fprintf(fid, "%.12e\n", isolution.GetPression(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < WallGhostCellIDs.size(); GhostID++) {
				int InteriorGhostIdx = WallAdjacentToGhostCellIDs[GhostID];
				fprintf(fid, "%.12e\n", isolution.GetEnergy(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < WallGhostCellIDs.size(); GhostID++) {
				int InteriorGhostIdx = WallAdjacentToGhostCellIDs[GhostID];
				fprintf(fid, "%.12e\n", isolution.GetV(InteriorGhostIdx));
			}

			// Connectivité des éléments de la partition
			for (size_t GhostID = 0; GhostID < WallGhostCellIDs.size(); GhostID++) {
				int FaceGhostIdx = WallAdjacentFaceIDs[GhostID];
				int nNode;
				int *ptr = iMesh.GetFace2NodeID(FaceGhostIdx, nNode);
				for (int iNode = 0; iNode < nNode; iNode++) {
					fprintf(fid, "%d\t", ptr[iNode] + 1);
				}
				fprintf(fid, "\n");
			}
		}
	}

	// Fermeture du fichier
	fclose(fid);
	return;
}
