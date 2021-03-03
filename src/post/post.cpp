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
	fprintf(fid, "VARIABLES=\"X\",\"Y\",\"Z\",\"Rho\",\"u\",\"v\",\"w\",\"p\",\"E\"\n");
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


void Post::WriteTecplotBinary() { return; }
