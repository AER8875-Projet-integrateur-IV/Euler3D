
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>


#include "parser/SU2MeshParser.hpp"

using E3D::Parser::SU2MeshParser;

SU2MeshParser::SU2MeshParser(const std::string &filename)
    : _filename(filename), _ifilestream(filename) {


	//File operation check
	if (!_ifilestream) {
		std::cerr << "Error while opening mesh file ! " << std::endl;
		exit(EXIT_FAILURE);
	}


	parseDim();
	parseVolumeElem();
	parsePoints();
	parseBC();

	_nElem = _nBoundaryElem + _nVolumeElem;
}


void SU2MeshParser::parseDim() {

	std::string line;
	bool ndim_found = false;

	while (std::getline(_ifilestream, line)) {

		if (line.find("NDIME=") != std::string::npos) {

			std::stringstream ss(line);

			ss.seekg(6) >> _nDim;

			ndim_found = true;
			break;
		}
	}

	if (!ndim_found) {
		std::cerr << "Dimension keyword not found \"NDIME=\" ! " << std::endl;
		exit(EXIT_FAILURE);
	}

	if (_nDim != 3) {
		std::cerr << "Geometry is not 3D (NDIME != 3 )" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void SU2MeshParser::parseVolumeElem() {
	std::string line;
	bool nelem_found = false;


	while (std::getline(_ifilestream, line)) {

		if (line.find("NELEM=") != std::string::npos) {

			std::stringstream ss(line);

			ss.seekg(6) >> _nVolumeElem;

			_InteriorElements.reserve(_nVolumeElem + 1);
			_InteriorElementsVtkID.reserve(_nVolumeElem + 1);
			_InteriorElementsFaceCount.reserve(_nVolumeElem + 1);

			nelem_found = true;

			// Temporary variables for loops
			int TempNodeID;
			int TempVtkID;
			std::vector<int> TempNodesSurrElem;

			for (int i = 0; i < _nVolumeElem; ++i) {
				std::getline(_ifilestream, line);
				std::stringstream ss1(line);
				ss1 >> TempVtkID;
				for (auto &[ID, nbNodes] : _vtkVolumeElements) {
					if (TempVtkID == ID) {
						// Populating InteriorElementsVtkID
						_InteriorElementsVtkID.push_back(TempVtkID);
						// Populating InteriorElementsFaceCount
						if ((nbNodes == 8) || (nbNodes == 6)) {
							nbNodes == 8 ? _InteriorElementsFaceCount.push_back(6)
							             : _InteriorElementsFaceCount.push_back(5);
						} else
							_InteriorElementsFaceCount.push_back(nbNodes);
						// Populating InteriorElements
						TempNodesSurrElem.reserve(nbNodes);
						for (int j = 0; j < nbNodes; j++) {
							ss1 >> TempNodeID;
							TempNodesSurrElem.push_back(TempNodeID);
						}
						_InteriorElements.emplace_back(Element(TempVtkID, TempNodesSurrElem));
						TempNodesSurrElem.clear();
						break;
					}
				}
			}
			break;
		}
	}

	if (!nelem_found) {
		std::cerr << "Dimension keyword not found \"NELEM=\" ! " << std::endl;
		exit(EXIT_FAILURE);
	}
}

void SU2MeshParser::parsePoints() {
	std::string line;
	bool npoint_found = false;

	while (std::getline(_ifilestream, line)) {

		if (line.find("NPOIN=") != std::string::npos) {

			std::stringstream ss(line);

			ss.seekg(6) >> _nPoints;

			npoint_found = true;

			_Points.reserve(_nPoints + 1);

			double TempX, TempY, TempZ;// Temporary variables
			std::vector<double> TempCoords;


			for (int i = 0; i < _nPoints; ++i) {
				TempCoords.reserve(3);
				std::getline(_ifilestream, line);
				std::stringstream ss1(line);
				for (int j = 0; j < 3; j++) {
					ss1 >> TempX >> TempY >> TempZ;
					TempCoords.push_back(TempX);
					TempCoords.push_back(TempY);
					TempCoords.push_back(TempZ);
				}
				_Points.emplace_back(Node(TempCoords));
				TempCoords.clear();
			}

			break;
		}
	}

	if (!npoint_found) {
		std::cerr << "Dimension keyword not found \"NPOIN=\" ! " << std::endl;
		exit(EXIT_FAILURE);
	}
}


void SU2MeshParser::parseBC() {
	std::string line;
	bool nmark_found = false;

	while (std::getline(_ifilestream, line)) {

		if (line.find("NMARK=") != std::string::npos) {

			std::stringstream ss(line);

			ss.seekg(6) >> _nMarkers;

			nmark_found = true;

			//Temporary variables
			std::vector<int> TempNodesSurrElem;
			int Temp_nTagElems;
			std::string Temp_TagName;
			std::pair<std::string, std::vector<Element>> TempBcPairWithAllElems;
			std::pair<std::string, int> TempBcPairWithnElems;
			std::vector<Element> TempTagElements;
			int TempVtkID;
			int TempNodeID;


			for (int i = 0; i < _nMarkers; i++) {

				// Parse MARKER_TAG
				std::getline(_ifilestream, line);
				std::stringstream ss1(line);
				ss1.seekg(11) >> Temp_TagName;

				//Parse MARKER_ELEMS
				std::getline(_ifilestream, line);
				std::stringstream ss2(line);
				ss2.seekg(13) >> Temp_nTagElems;

				// updating variables to parse current tag elements
				_nBoundaryElem += Temp_nTagElems;
				TempBcPairWithAllElems.first = Temp_TagName;
				TempBcPairWithnElems.first = Temp_TagName;
				TempBcPairWithnElems.second = Temp_nTagElems;
				_tags.push_back(TempBcPairWithnElems);
				TempTagElements.reserve(Temp_nTagElems);


				for (int i = 0; i < Temp_nTagElems; i++) {
					std::getline(_ifilestream, line);
					std::stringstream ss3(line);
					ss3 >> TempVtkID;
					for (auto &[ID, nbNodes] : _vtkSurfaceElements) {
						if (TempVtkID == ID) {
							TempNodesSurrElem.reserve(nbNodes);
							for (int j = 0; j < nbNodes; j++) {
								ss3 >> TempNodeID;
								TempNodesSurrElem.push_back(TempNodeID);
							}
							TempTagElements.emplace_back(Element(TempVtkID, TempNodesSurrElem));
							TempNodesSurrElem.clear();
							break;
						}
					}
				}
				TempBcPairWithAllElems.second = TempTagElements;
				_BoundaryElements.push_back(TempBcPairWithAllElems);
				TempTagElements.clear();
			}
			break;
		}
	}
	if (!nmark_found) {
		std::cerr << "Dimension keyword not found \"NMARK=\" ! " << std::endl;
		exit(EXIT_FAILURE);
	}
}

void SU2MeshParser::printInfo() {

	// print mesh file name
	std::cout << "\n"
	          << std::setw(30)
	          << "Mesh File Name : "
	          << std::setw(6)
	          << _filename
	          << "\n";

	std::cout << std::setw(30)
	          << "Number of Volume Elements : "
	          << std::setw(6)
	          << _nVolumeElem
	          << "\n";

	std::cout << std::setw(30)
	          << "Number of Nodes : "
	          << std::setw(6)
	          << _nPoints
	          << "\n";

	std::cout << std::setw(30)
	          << "Number of BC Elements : "
	          << std::setw(6)
	          << _nBoundaryElem
	          << "\n";

	std::cout << std::setw(30)
	          << "Number of BC Markers : "
	          << std::setw(6)
	          << _nMarkers
	          << "\n\n";

	for (auto &[tag, count] : _tags) {
		std::cout << std::setw(20) << "Tag: " << std::setw(10) << tag << " | Number of elements: " << count
		          << std::endl;
	}


	// print total count of elements
	std::cout << "\n"
	          << std::setw(30)
	          << "Total Number of Elements : "
	          << std::setw(6)
	          << _nElem
	          << "\n\n";
}