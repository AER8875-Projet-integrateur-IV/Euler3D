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
#include <algorithm>
#include <utility>


using namespace E3D::Post;


Post::Post(std::vector<std::string> pathPartition, std::string outputFile) {
	_outputFile = outputFile;
	_nPart = pathPartition.size();
	_meshPartitionPath.reserve(_nPart);
	_solutionPartitionPath.reserve(_nPart);
	for (int i = 0; i < _nPart; i++) {
		std::string path = pathPartition[i];
		_meshPartitionPath.push_back(path + ".par");
		_solutionPartitionPath.push_back(path + ".sol");
	}
	return;
}

Post::~Post() {
	return;
}

void Post::Write() {
	std::cout << "Début Post:\n";
	WriteTecplotASCII();
	std::cout << "Fin Post:\n";
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
	char *Title = "Titre du fichier";
	char *Variables = "X,Y,Z,Rho,U,V,W,P,E";
	std::string fileName = "./" + _outputFile + ".plt";
	char *FName = new char[_outputFile.length() + 1];
	strcpy(FName, _outputFile.c_str());
	char *ScratchDir = ".";
	INTEGER4 FileFormat = 0;// 0 = .plt et 1 = .szplt
	INTEGER4 FileType = 0;  // 0=FUll (mesh+solution)
	INTEGER4 Debug = 1;     // mode debug activé
	INTEGER4 VIsDouble = 1; // precision double
	INTEGER4 I = 0;         // retour de la fonction tecplot

	I = tecini142(Title, Variables, FName, ScratchDir, &FileFormat, &FileType, &Debug, &VIsDouble);
	delete[] Title;
	delete[] Variables;
	delete[] FName;
	delete[] ScratchDir;

	// Écriture des zones
	// Initialisation de la zone
	int iPart(0);
	E3D::Parser::SU2MeshParser iMesh = E3D::Parser::SU2MeshParser(_meshPartitionPath[iPart]);
	char *ZoneTitle = "Titre de la zone";
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
	delete[] ZoneTitle;

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
		for (int iNode = 0; iNode < 8; iNode++) {
			NData[iNode + iElem * 8] = elem[iElem].getElemNodes()[iNode] + 1;
		}
	}

	I = tecnode142(&N, NData);
	delete[] NData;

	// Fermeture du fichier binaire
	I = tecend142();

	return;
}
