
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>


#include "parser/SolutionPost.hpp"

using E3D::Parser::SolutionPost;

SolutionPost::SolutionPost(std::string filename, int nElem)
    : _filename(filename), _ifilestream(filename), _nElem(nElem) {


	//File operation check
	if (!_ifilestream) {
		std::cerr << "Error while opening solution file ! " << std::endl;
		exit(EXIT_FAILURE);
	}
	_rho.reserve(nElem);
	_u.reserve(nElem);
	_v.reserve(nElem);
	_w.reserve(nElem);
	_p.reserve(nElem);
	_e.reserve(nElem);


	parseFile();
}

SolutionPost::~SolutionPost() {
	_ifilestream.close();
	return;
}

void SolutionPost::parseFile() {

	std::string line;
	std::getline(_ifilestream, line);// Saut de la première ligne
	double rho, u, v, w, p, e;
	for (int i = 0; i < _nElem; i++) {
		std::getline(_ifilestream, line);
		std::stringstream ss1(line);
		ss1 >> rho;
		_rho.push_back(rho);
		ss1 >> u;
		_u.push_back(u);
		ss1 >> v;
		_v.push_back(v);
		ss1 >> w;
		_w.push_back(w);
		ss1 >> p;
		_p.push_back(p);
		ss1 >> e;
		_e.push_back(e);
	}
	return;
}
