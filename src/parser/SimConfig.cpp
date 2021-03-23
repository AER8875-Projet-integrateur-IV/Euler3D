//
// Created by amin on 2/4/21.
//


#include "parser/SimConfig.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <regex>
#include <stdexcept>

using namespace E3D::Parser;

SimConfig::SimConfig(const std::string &filename, const int rankID, const int poolSize)
    : _configFileStream(filename), _configFile(filename) {

	if (!_configFileStream) {
		std::cerr << "Error while opening configuration file ! "
		          << "\n";
		exit(EXIT_FAILURE);
	}

	parseConfigFile();
	if (rankID == 0) {
		if (_nbPartition != poolSize) {
			printf("Number of mesh files and MPI processes are not equal !\n");
			printf("Number of partition = %i | Number of MPI processes : %i \n", _nbPartition,
			       poolSize);
			exit(EXIT_FAILURE);
		}
		printInfo();
	}
}

SimConfig::SimConfig(const std::string &filename)
    : _configFileStream(filename), _configFile(filename) {

	if (!_configFileStream) {
		std::cerr << "Error while opening configuration file ! "
		          << "\n";
		exit(EXIT_FAILURE);
	}

	parseConfigFile();
}

void SimConfig::parseConfigFile() {
	int meshOrientationCL;
	int meshOrientationCD;
	std::string line;

	while (std::getline(_configFileStream, line)) {

		if (line.find("START") != std::string::npos) {
			while (!(line.find("END") != std::string::npos)) {
				std::getline(_configFileStream, line);

				std::stringstream ss1(line);

				if (line[0] == '#') {
					continue;
				} else if (line.find("INITIAL_MESH=") != std::string::npos) {
					ss1.seekg(14) >> _initialMeshFile;
				} else if (line.find("PARTITION_FILES=") != std::string::npos) {

					// Parse number of patition and go next line
					ss1.seekg(16) >> _nbPartition;
					std::getline(_configFileStream, line);

					// vector to hold partitions file names
					std::vector<std::string> partitionMeshes;
					std::stringstream ss2(line);

					// string to hold partition file name with #
					std::string tempHashtagPartitionName;
					ss2 >> tempHashtagPartitionName;

					// Replacing # with partition number
					for (int i = 0; i < _nbPartition; i++) {
						std::string tempModifiedPartitionName = tempHashtagPartitionName;
						tempModifiedPartitionName = std::regex_replace(tempModifiedPartitionName, std::regex("\\#"), std::to_string(i));
						partitionMeshes.push_back(tempModifiedPartitionName);
					}
					_meshFiles = partitionMeshes;
				} else if (line.find("MESH_ORIENTATION_CL=") != std::string::npos) {
					ss1.seekg(21) >> meshOrientationCL;
				} else if (line.find("MESH_ORIENTATION_CD=") != std::string::npos) {
					ss1.seekg(21) >> meshOrientationCD;
				} else if (line.find("MESH_REF_POINT_X=") != std::string::npos) {
					ss1.seekg(18) >> _meshRefPoint.x;
				} else if (line.find("MESH_REF_POINT_Y=") != std::string::npos) {
					ss1.seekg(18) >> _meshRefPoint.y;
				} else if (line.find("MESH_REF_POINT_Z=") != std::string::npos) {
					ss1.seekg(18) >> _meshRefPoint.z;
				} else if (line.find("SAMPLING=") != std::string::npos) {
					ss1.seekg(10) >> _samplingPeriod;
				} else if (line.find("PRE_LOG=") != std::string::npos) {
					ss1.seekg(9) >> _preProcessorLog;
				} else if (line.find("SPEED_OPTION") != std::string::npos) {
					int speedChoice;
					ss1.seekg(13) >> speedChoice;
					speedChoice == 0 ? _spdoption = SpeedType::MACH : _spdoption = SpeedType::METER_PER_SEC;
				} else if (line.find("SPEED_VALUE") != std::string::npos) {
					ss1.seekg(12) >> _mach;
				} else if (line.find("AOA") != std::string::npos) {
					ss1.seekg(4) >> _aoa;

				} else if (line.find("AIRFLOW_PRESSURE") != std::string::npos) {
					ss1.seekg(17) >> _pressure;
				} else if (line.find("AIRFLOW_TEMPERATURE") != std::string::npos) {
					ss1.seekg(20) >> _temp;
				} else if (line.find("AIRFLOW_VISCOSITY") != std::string::npos) {
					ss1.seekg(18) >> _visc;
				} else if (line.find("AIRFLOW_DENSITY") != std::string::npos) {
					ss1.seekg(16) >> _density;
				} else if (line.find("GAMMA") != std::string::npos) {
					ss1.seekg(6) >> _gamma;
				} else if (line.find("GAS_CONSTANT") != std::string::npos) {
					ss1.seekg(13) >> _gasConstant;
				} else if (line.find("SPECIFIC_HEAT") != std::string::npos) {
					ss1.seekg(14) >> _specificHeat;
				} else if (line.find("SCHEME") != std::string::npos) {
					int schemeChoice;
					ss1.seekg(8) >> schemeChoice;
					schemeChoice == 0 ? _solverScheme = SolverScheme::ROE : _solverScheme = SolverScheme::AUSM;
				} else if (line.find("TIME_INTEGRATION") != std::string::npos) {
					int timeIntegrationChoice;

					ss1.seekg(17) >> timeIntegrationChoice;

					switch (timeIntegrationChoice) {
						case 0:
							_temporalScheme = TemporalScheme::EXPLICIT_EULER;
							break;
						case 1:
							_temporalScheme = TemporalScheme::IMPLICIT_EULER;
							break;
						case 2:
							_temporalScheme = TemporalScheme::RK5;
							break;
					}
				} else if (line.find("RESIDUAL_SMOOTHING") != std::string::npos) {

					ss1.seekg(19) >> _residualSmoothing;
					if (_residualSmoothing == 0)
						_residualSmoothing = false;
					else
						_residualSmoothing = true;

				} else if (line.find("CFL") != std::string::npos) {
					ss1.seekg(4) >> _cfl;
				} else if (line.find("MIN_RESIDUAL") != std::string::npos) {
					ss1.seekg(13) >> _minResidual;
				} else if (line.find("MIN_AEROCOEFF_ERROR") != std::string::npos) {
					ss1.seekg(20) >> _minAeroCoeffError;
				} else if (line.find("MAX_ITER") != std::string::npos) {
					ss1.seekg(9) >> _maxIter;
				} else if (line.find("SOLVER_LOG=") != std::string::npos) {
					ss1.seekg(11) >> _solverLog;
				} else if (line.find("OUTPUT_FILE") != std::string::npos) {
					ss1.seekg(13) >> _tecplotFile;
				} else if (line.find("POST_LOG=") != std::string::npos) {
					ss1.seekg(9) >> _postProcessorLog;
				}
			}
			break;
		}
	}
	if (_spdoption == SpeedType::MACH) {
		_velocity = std::sqrt(_gamma * _gasConstant * _temp) * _mach;

	} else if (_spdoption == SpeedType::METER_PER_SEC) {
		_velocity = _mach;
		_mach = _velocity / (std::sqrt(_gamma * _gasConstant * _temp));
	}

	// Solve mesh orientation
	switch (meshOrientationCL) {
		case 0:
			_meshOrientationCL = std::make_pair(0, 1);
			break;
		case 1:
			_meshOrientationCL = std::make_pair(0, -1);
			break;
		case 2:
			_meshOrientationCL = std::make_pair(1, 1);
			break;
		case 3:
			_meshOrientationCL = std::make_pair(1, -1);
			break;
		case 4:
			_meshOrientationCL = std::make_pair(2, 1);
			break;
		case 5:
			_meshOrientationCL = std::make_pair(2, -1);
			break;
		default:
			throw std::invalid_argument("invalid MESH_ORIENTATION_CL argument");
			break;
	};
	switch (meshOrientationCD) {
		case 0:
			_meshOrientationCD = std::make_pair(0, 1);
			break;
		case 1:
			_meshOrientationCD = std::make_pair(0, -1);
			break;
		case 2:
			_meshOrientationCD = std::make_pair(1, 1);
			break;
		case 3:
			_meshOrientationCD = std::make_pair(1, -1);
			break;
		case 4:
			_meshOrientationCD = std::make_pair(2, 1);
			break;
		case 5:
			_meshOrientationCD = std::make_pair(2, -1);
			break;
		default:
			throw std::invalid_argument("invalid MESH_ORIENTATION_CD argument");
			break;
	};
	_meshOrientationCM = std::make_pair(3 - _meshOrientationCL.first - _meshOrientationCD.first, -_meshOrientationCD.second * _meshOrientationCD.second);

	// Define standard output Dir
	std::filesystem::path outputPath(_tecplotFile);
	_outputDir = outputPath.parent_path();
}

void SimConfig::printInfo() {

	std::cout << std::string(24, '#') << "  SimConfig  " << std::string(24, '#') << "\n\n";

	// Print name of config file
	std::cout << "\n"
	          << std::setw(30)
	          << "Configuration File : "
	          << std::setw(6)
	          << _configFile
	          << "\n";

	// Print number of partitions
	std::cout << "\n"
	          << std::setw(30)
	          << "Number of partitions : "
	          << std::setw(6)
	          << _nbPartition
	          << "\n";

	// Print mach value
	std::cout << "\n"
	          << std::setw(30)
	          << "Mach : "
	          << std::setw(6)
	          << _mach
	          << "\n";

	// Print angle of attack
	std::cout << "\n"
	          << std::setw(30)
	          << "Angle of attack (deg): "
	          << std::setw(6)
	          << _aoa
	          << "\n";
}
