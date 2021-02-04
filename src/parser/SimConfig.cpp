//
// Created by amin on 2/4/21.
//



#include "parser/SimConfig.hpp"
#include <iostream>
#include <math.h>

using namespace E3D::Parser;

SimConfig::SimConfig(std::string filename) : _configFileStream(filename), _configFile(filename) {

    std::cout << std::string(24, '#') << "  SimConfig  " << std::string(24, '#') << "\n\n";


    if (!_configFileStream) {
        std::cerr << "Error while opening configuration file ! " << "\n";
        exit(EXIT_FAILURE);
    }

    parseConfigFile();
}

void SimConfig::parseConfigFile() {

    std::string line;

    while (std::getline(_configFileStream, line)) {

        if (line.find("START") != std::string::npos) {
            while (!(line.find("END") != std::string::npos)) {
                std::getline(_configFileStream, line);

                std::stringstream ss1(line);

                if (line[0] == '#') {
                    continue;
                } else if (line.find("PARTITION_FILES=") != std::string::npos) {
                    std::vector<std::string> partitionMeshes;
                    std::string tempPartitionName;
                    ss1.seekg(16) >> _nbPartition;
                    for (int i = 0; i < _nbPartition; i++) {

                        std::getline(_configFileStream, line);
                        std::stringstream ss2(line);
                        ss2 >> tempPartitionName;
                        partitionMeshes.push_back(tempPartitionName);

                    }
                    _meshFiles = partitionMeshes;


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
                } else if (line.find("CFL") != std::string::npos) {
                    ss1.seekg(4) >> _cfl;
                } else if (line.find("MIN_RESIDUAL") != std::string::npos) {
                    ss1.seekg(13) >> _minResidual;
                } else if (line.find("MAX_ITER") != std::string::npos) {
                    ss1.seekg(9) >> _maxIter;
                } else if (line.find("OUTPUT_FILE") != std::string::npos) {
                    ss1.seekg(13) >> _tecplotFile;
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

}