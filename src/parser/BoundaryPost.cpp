
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>


#include "parser/BoundaryPost.hpp"

using E3D::Parser::BoundaryPost;

BoundaryPost::BoundaryPost(std::string filename)
    : _filename(filename), _ifilestream(filename) {


	//File operation check
	if (!_ifilestream) {
		std::cerr << "Error while opening boundary file ! " << std::endl;
		exit(EXIT_FAILURE);
	}

	parseFile();
}

BoundaryPost::~BoundaryPost() {
	_ifilestream.close();
	return;
}

void BoundaryPost::parseFile() {

	std::string line;
	std::getline(_ifilestream, line);
	std::stringstream ss(line);
	ss.seekg(20) >> _nWallCells;
	_adjacentElement.reserve(_nWallCells);
	_nodeStart.reserve(_nWallCells + 1);
	_nodeStart.push_back(0);
	_node.reserve(_nWallCells * 4);
	int elem, nNodes, nodeI;
	for (int i = 0; i < _nWallCells; i++) {
		std::getline(_ifilestream, line);
		std::stringstream ss1(line);
		ss1 >> elem;
		_adjacentElement.push_back(elem);
		ss1 >> nNodes;
		_nodeStart.push_back(_nodeStart.back() + nNodes);
		for (int i = 0; i < nNodes; i++) {
			ss1 >> nodeI;
			_node.push_back(nodeI);
		}
	}
	return;
}
