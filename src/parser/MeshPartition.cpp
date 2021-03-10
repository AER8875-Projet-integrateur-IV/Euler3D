//
// Created by amin on 2/5/21.
//


#include "parser/MeshPartition.hpp"
#include <array>
#include <iostream>
#include <mpi.h>
#include <sstream>


using namespace E3D::Parser;

MeshPartition::MeshPartition(const std::string &fileName, const int RankID)
    : SU2MeshParser(fileName) {

	_rankID = RankID;

	double startParserTimer = MPI_Wtime();

	if (RankID == 0) {
		std::cout << "\n\n"
		          << std::string(24, '#') << "  MeshPartition  " << std::string(24, '#') << "\n\n";
	}
	parseMpiBoundaryElement();


	MPI_Barrier(MPI_COMM_WORLD);
	if (RankID == 0) {
		double endParserTimer = MPI_Wtime();
		printf("All processes parsed mesh files !\n");
		printf("Mesh Partitions took %.5f seconds to parse.\n", endParserTimer - startParserTimer);
	}

	_ifilestream.close();
}

void MeshPartition::parseMpiBoundaryElement() {
	std::string line;

	bool mpi_BC_found = false;

	while (std::getline(_ifilestream, line)) {
		if (line.find("NINTERNAL_MARKER=") != std::string::npos) {

			mpi_BC_found = true;

			std::stringstream ss(line);

			ss.seekg(17) >> _nbAdjacentPartitions;

			// Temporary variables for next for-loop
			int temp_nbElemsBetweenPartitions;
			int temp_partitionID;
			int temp_thiselemID;
			int temp_otherElemID;
			int temp_faceVtkID;
			int temp_oneFaceNode;


			for (int nbPartitions = 0; nbPartitions < _nbAdjacentPartitions; nbPartitions++) {
				std::pair<int, std::vector<Parser::GhostCell>> temp_MpiBoundaryElems;

				// read INTERNAL_MARKER
				std::getline(_ifilestream, line);
				std::stringstream ss1(line);
				ss1.seekg(16) >> temp_partitionID;
				_adjacentPartitionsID.push_back(temp_partitionID);
				temp_MpiBoundaryElems.first = temp_partitionID;


				// read NELEM_MPI
				std::getline(_ifilestream, line);
				std::stringstream ss2(line);
				ss2.seekg(10) >> temp_nbElemsBetweenPartitions;

				_nbMpiBoundaryElements += temp_nbElemsBetweenPartitions;
				temp_MpiBoundaryElems.second.reserve(temp_nbElemsBetweenPartitions);


				// loop through MPI boundary elements
				for (int nbMPIelems = 0; nbMPIelems < temp_nbElemsBetweenPartitions; nbMPIelems++) {
					std::vector<int> temp_faceNodes;
					std::getline(_ifilestream, line);
					std::stringstream ss3(line);
					ss3 << line;
					ss3 >> temp_thiselemID >> temp_otherElemID >> temp_faceVtkID;
					for (auto &[ID, nbNodes] : _vtkSurfaceElements) {
						if (temp_faceVtkID == ID) {
							for (int i = 0; i < nbNodes; i++) {
								ss3 >> temp_oneFaceNode;
								temp_faceNodes.push_back(temp_oneFaceNode);
							}
							break;
						}
					}
					temp_MpiBoundaryElems.second.emplace_back(E3D::Parser::GhostCell(temp_thiselemID,
					                                                                 temp_otherElemID,
					                                                                 temp_faceVtkID,
					                                                                 temp_faceNodes));
				}
				_MpiBoundaryElements.push_back(temp_MpiBoundaryElems);
			}
			break;
		}
	}
	if (!mpi_BC_found) {
		std::cerr << "MPI Boundary Elements keyword \"NINTERNAL_MARKER=\" not found ! " << std::endl;
		exit(EXIT_FAILURE);
	}
}

void MeshPartition::printAllPartitionsInfo() {

	std::array<int, 4> meshStats = {_nVolumeElem, _nBoundaryElem, _nbMpiBoundaryElements, _nPoints};
	std::array<int, 4> sumMeshStats{0, 0, 0, 0};

	MPI_Reduce(&meshStats, &sumMeshStats, 4, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (_rankID == 0) {

		printf("\nTotal Number of Volume Elements : %d \n", sumMeshStats[0]);
		printf("Total Number of Surface Elements : %d \n", sumMeshStats[1]);
		printf("Total Number of MPI Ghost Cells : %d \n", sumMeshStats[2]);
		printf("Total Number of Nodes (including duplicates) : %d \n", sumMeshStats[3]);
	}

	MPI_Barrier(MPI_COMM_WORLD);
}
