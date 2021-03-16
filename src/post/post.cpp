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
	CoeffPression(config);
	return;
}

Post::~Post() {
	return;
}

void Post::CoeffPression(const E3D::Parser::SimConfig &config) {
	const double E3D_PI = 3.141592653589793238;
	double gamma_ref = config.getGamma();
	double M_inf = config.getMach();
	double rho_inf = 1.0;
	_CpFactor = 1. / (0.5 * rho_inf * (M_inf * M_inf * gamma_ref));
}

void Post::Write() {
	std::cout << std::string(24, '#') << "  TECPLOT  " << std::string(24, '#') << "\n"
	          << std::endl;
	auto logger = E3D::Logger::Getspdlog();

	// Tecplot ASCII
	std::cout << "Écriture du fichier Tecpot (ASCII) ..." << std::endl;
	spdlog::stopwatch tecplotASCIIsw;
	WriteTecplotASCII();
	std::cout << "Output ASCII File: " << _outputFile << std::endl;
	logger->debug("Writing Tecplot ASCII file run time {}", tecplotASCIIsw);

	// Tecplot Binaire
	std::cout << "Écriture du fichier Tecpot (Binaire) ..." << std::endl;
	spdlog::stopwatch tecplotBinarysw;
	WriteTecplotBinary();
	std::cout << "Output Binary File: " << _outputFile + ".plt" << std::endl;
	logger->debug("Writing Tecplot Binary file run time {}", tecplotBinarysw);

	// Tecplot Surface ASCII
	std::cout << "Écriture du fichier Tecpot Surface (ASCII) ..." << std::endl;
	spdlog::stopwatch tecplotSurfaceASCIIsw;
	WriteTecplotSurfaceASCII();
	std::cout << "Output (surface) ASCII File: " << _outputFile + ".surf.dat" << std::endl;
	logger->debug("Writing Tecplot Surface ASCII file run time {}", tecplotSurfaceASCIIsw);

	// Tecplot Surface Binaire
	std::cout << "Écriture du fichier Tecpot Surface (Binaire) ..." << std::endl;
	spdlog::stopwatch tecplotSurfaceBinarysw;
	WriteTecplotSurfaceBinary();
	std::cout << "Output (Surface) Binary File: " << _outputFile + ".surf.plt" << std::endl;
	logger->debug("Writing Tecplot Surface Binary file run time {}", tecplotSurfaceBinarysw);
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
	std::string outputFile = _outputFile + ".plt";
	char const *FName = outputFile.c_str();
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
		E3D::Parser::SU2MeshParser iMesh = E3D::Parser::SU2MeshParser(_meshPartitionPath[iPart]);
		E3D::Parser::BoundaryPost iBoundary(_meshPartitionPath[iPart] + ".bry");
		// EEntête de la zone
		int nNodes = iBoundary.GetWallNodeCount();
		int nElements = iBoundary.GetWallElementCount();
		if (nElements != 0) {
			fprintf(fid, "ZONE T=\"Element\"\nNodes=%d, Elements=%d, ZONETYPE=FEQUADRILATERAL\nDATAPACKING=BLOCK, VARLOCATION=([4-10]=CELLCENTERED)\n", nNodes, nElements);

			// Coordonnées des noeuds de la partition
			for (int nodeI = 0; nodeI < nNodes; nodeI++) {
				int nodeG = iBoundary.GetWallGlobalNode(nodeI);
				E3D::Parser::Node node = iMesh.GetPoints()[nodeG];
				fprintf(fid, "%.12e\n", node.getX());
			}
			for (int nodeI = 0; nodeI < nNodes; nodeI++) {
				int nodeG = iBoundary.GetWallGlobalNode(nodeI);
				E3D::Parser::Node node = iMesh.GetPoints()[nodeG];
				fprintf(fid, "%.12e\n", node.getY());
			}
			for (int nodeI = 0; nodeI < nNodes; nodeI++) {
				int nodeG = iBoundary.GetWallGlobalNode(nodeI);
				E3D::Parser::Node node = iMesh.GetPoints()[nodeG];
				fprintf(fid, "%.12e\n", node.getZ());
			}

			// Lecture des solutions
			E3D::Parser::SolutionPost isolution(_solutionPartitionPath[iPart], iMesh.GetVolumeElemCount());

			for (size_t GhostID = 0; GhostID < nElements; GhostID++) {
				int InteriorGhostIdx = iBoundary.GetAdjacentCell(GhostID);
				fprintf(fid, "%.12e\n", isolution.GetRho(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < nElements; GhostID++) {
				int InteriorGhostIdx = iBoundary.GetAdjacentCell(GhostID);
				fprintf(fid, "%.12e\n", isolution.GetU(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < nElements; GhostID++) {
				int InteriorGhostIdx = iBoundary.GetAdjacentCell(GhostID);
				fprintf(fid, "%.12e\n", isolution.GetV(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < nElements; GhostID++) {
				int InteriorGhostIdx = iBoundary.GetAdjacentCell(GhostID);
				fprintf(fid, "%.12e\n", isolution.GetW(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < nElements; GhostID++) {
				int InteriorGhostIdx = iBoundary.GetAdjacentCell(GhostID);
				fprintf(fid, "%.12e\n", isolution.GetPression(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < nElements; GhostID++) {
				int InteriorGhostIdx = iBoundary.GetAdjacentCell(GhostID);
				fprintf(fid, "%.12e\n", isolution.GetEnergy(InteriorGhostIdx));
			}
			for (size_t GhostID = 0; GhostID < nElements; GhostID++) {
				int InteriorGhostIdx = iBoundary.GetAdjacentCell(GhostID);
				double Cp = _CpFactor * (isolution.GetPression(InteriorGhostIdx) - _p_inf);
				fprintf(fid, "%.12e\n", Cp);
			}

			// Connectivité des éléments de la partition
			for (size_t GhostID = 0; GhostID < nElements; GhostID++) {
				int nNode;
				const int *ptr = iBoundary.GetWallNodes(GhostID, nNode);
				if (nNode == 3)// triangle
				{
					fprintf(fid, "%d\t%d\t%d\t%d\n",
					        ptr[0] + 1, ptr[1] + 1, ptr[2] + 1, ptr[2] + 1);
				} else {
					for (int iNode = 0; iNode < nNode; iNode++) {
						fprintf(fid, "%d\t", ptr[iNode] + 1);
					}
					fprintf(fid, "\n");
				}
			}
		}
	}

	// Fermeture du fichier
	fclose(fid);
	return;
}

void Post::WriteTecplotSurfaceBinary() {
	//Initialisation du fichier binaire
	char const *Title = "Titre du fichier";
	char const *Variables = "X,Y,Z,Rho,U,V,W,P,E,Cp";
	std::string outputFile = _outputFile + ".surf.plt";
	char const *FName = outputFile.c_str();
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
		E3D::Parser::BoundaryPost iBoundary(_meshPartitionPath[iPart] + ".bry");
		int nElements = iBoundary.GetWallElementCount();
		if (nElements != 0) {
			std::string titreZone = "Partition " + std::to_string(iPart);
			char const *ZoneTitle = titreZone.c_str();
			INTEGER4 ZoneType = 3;                                      // 3 = FEQUADRILATERAL
			INTEGER4 NumPts = iBoundary.GetWallNodeCount();             // Nombre de noeuds de la zone
			INTEGER4 NumElements = nElements;                           // Nombre d'élements
			INTEGER4 NumFaces(0), ICellMax(0), JCellMax(0), KCellMax(0);// Non utilisé
			double SolutionTime = 0.;
			INTEGER4 StrandID = 0;  // zone statique
			INTEGER4 ParentZone = 0;// pas de zone parent
			INTEGER4 IsBlock = 1;
			INTEGER4 NumFaceConnections(0), FaceNeighborMode(0), TotalNumFaceNodes(0);// Non utilisé
			INTEGER4 NumConnectedBoundaryFaces(0), TotalNumBoundaryConnections(0);    // Non utilisé
			INTEGER4 ValueLocation[] = {1, 1, 1, 0, 0, 0, 0, 0, 0, 0};                // 0 = cell-centered et 1 = node-centered
			INTEGER4 ShareConnectivityFromZone = 0;

			I = teczne142(ZoneTitle, &ZoneType, &NumPts, &NumElements, &NumFaces, &ICellMax, &JCellMax, &KCellMax,
			              &SolutionTime, &StrandID, &ParentZone, &IsBlock, &NumFaceConnections, &FaceNeighborMode,
			              &TotalNumFaceNodes, &NumConnectedBoundaryFaces, &TotalNumBoundaryConnections, NULL,
			              ValueLocation, NULL, &ShareConnectivityFromZone);

			// Récupération des variables
			double *X = new double[NumPts];
			double *Y = new double[NumPts];
			double *Z = new double[NumPts];
			double *Rho = new double[NumElements];
			double *U = new double[NumElements];
			double *V = new double[NumElements];
			double *W = new double[NumElements];
			double *P = new double[NumElements];
			double *E = new double[NumElements];
			double *Cp = new double[NumElements];
			for (int nodeI = 0; nodeI < NumPts; nodeI++) {
				int nodeG = iBoundary.GetWallGlobalNode(nodeI);
				E3D::Parser::Node node = iMesh.GetPoints()[nodeG];
				X[nodeI] = node.getX();
				Y[nodeI] = node.getY();
				Z[nodeI] = node.getZ();
			}
			E3D::Parser::SolutionPost isolution(_solutionPartitionPath[iPart], NumElements);
			for (size_t GhostID = 0; GhostID < nElements; GhostID++) {
				int InteriorGhostIdx = iBoundary.GetAdjacentCell(GhostID);
				Rho[GhostID] = isolution.GetRho(InteriorGhostIdx);
				U[GhostID] = isolution.GetU(InteriorGhostIdx);
				V[GhostID] = isolution.GetV(InteriorGhostIdx);
				W[GhostID] = isolution.GetW(InteriorGhostIdx);
				P[GhostID] = isolution.GetPression(InteriorGhostIdx);
				E[GhostID] = isolution.GetEnergy(InteriorGhostIdx);
				Cp[GhostID] = _CpFactor * (isolution.GetPression(InteriorGhostIdx) - _p_inf);
			}

			// Écriture des variables de la zone
			I = tecdat142(&NumPts, X, &VIsDouble);
			I = tecdat142(&NumPts, Y, &VIsDouble);
			I = tecdat142(&NumPts, Z, &VIsDouble);

			I = tecdat142(&NumElements, Rho, &VIsDouble);
			I = tecdat142(&NumElements, U, &VIsDouble);
			I = tecdat142(&NumElements, V, &VIsDouble);
			I = tecdat142(&NumElements, W, &VIsDouble);
			I = tecdat142(&NumElements, P, &VIsDouble);
			I = tecdat142(&NumElements, E, &VIsDouble);
			I = tecdat142(&NumElements, Cp, &VIsDouble);

			delete[] X;
			delete[] Y;
			delete[] Z;
			delete[] U;
			delete[] V;
			delete[] W;
			delete[] P;
			delete[] E;
			delete[] Cp;

			// Connectivité de la zone
			INTEGER4 N = 4 * NumElements;// FEQUADRILATERAL : chaque elément a 4 noeuds
			INTEGER4 *NData = new INTEGER4[N];

			for (int iElem = 0; iElem < NumElements; iElem++) {
				int nNode;
				const int *ptr = iBoundary.GetWallNodes(iElem, nNode);
				if (nNode == 3)// triangle
				{
					NData[iElem * 4 + 0] = ptr[0] + 1;
					NData[iElem * 4 + 1] = ptr[1] + 1;
					NData[iElem * 4 + 2] = ptr[2] + 1;
					NData[iElem * 4 + 3] = ptr[2] + 1;

				} else {
					NData[iElem * 4 + 0] = ptr[0] + 1;
					NData[iElem * 4 + 1] = ptr[1] + 1;
					NData[iElem * 4 + 2] = ptr[2] + 1;
					NData[iElem * 4 + 3] = ptr[3] + 1;
				}
			}

			I = tecnode142(&N, NData);
			delete[] NData;
		}
	}

	// Fermeture du fichier binaire
	I = tecend142();
	return;
}