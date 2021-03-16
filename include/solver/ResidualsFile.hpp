#pragma once

#include "utils/Vector3.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

class ResidualsFile {
private:
	std::ofstream _file;
	int _columnWidth;
	int _precision;

public:
	ResidualsFile(const std::filesystem::path &path, int columnWidth = 30, int precision = 10) : _columnWidth(columnWidth), _precision(precision) {
		_file.open(path);
		_file << std::setw(_columnWidth) << "Max residual"
		      << std::setw(_columnWidth) << "Cx"
		      << std::setw(_columnWidth) << "Cy"
		      << std::setw(_columnWidth) << "Cz"
		      << std::setw(_columnWidth) << "CMx"
		      << std::setw(_columnWidth) << "CMy"
		      << std::setw(_columnWidth) << "CMz"
		      << std::endl;
		;
	}

	ResidualsFile() = default;

	~ResidualsFile() {
		// close residuals file
		_file.close();
	}

	void Update(double error, const E3D::Vector3<double> &forceCoeff, const E3D::Vector3<double> &momentCoeff) {
		_file << std::scientific << std::setprecision(_precision) << std::setw(_columnWidth) << error
		      << std::scientific << std::setprecision(_precision) << std::setw(_columnWidth) << forceCoeff.x
		      << std::scientific << std::setprecision(_precision) << std::setw(_columnWidth) << forceCoeff.y
		      << std::scientific << std::setprecision(_precision) << std::setw(_columnWidth) << forceCoeff.z
		      << std::scientific << std::setprecision(_precision) << std::setw(_columnWidth) << momentCoeff.x
		      << std::scientific << std::setprecision(_precision) << std::setw(_columnWidth) << momentCoeff.y
		      << std::scientific << std::setprecision(_precision) << std::setw(_columnWidth) << momentCoeff.z
		      << std::endl;
	}
};