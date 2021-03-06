#pragma once

#include "utils/Vector3.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace E3D::Parser {


	class SimConfig {
	public:
		SimConfig(const std::string &filename, const int rankID, const int poolSize);
		SimConfig(const std::string &filename);

		void parseConfigFile();

		void printInfo();

		void configFileVerification();

		enum class SpeedType {
			MACH,
			METER_PER_SEC
		};

		enum class SolverScheme {
			ROE,
			AUSM
		};

		enum class TemporalScheme {
			EXPLICIT_EULER,
			RK5,
			IMPLICIT_EULER
		};

	public:
		/**
         * @return Vector of strings of mesh file names
         */
		[[nodiscard]] inline const std::vector<std::string> &getPartitionedMeshFiles() const { return _meshFiles; }

		/**
         * @return tecplot output file path (relative to executable) and name
         */
		[[nodiscard]] inline const std::string &getTecplotFile() const { return _tecplotFile; }

		/**
         * @return Angle of attack in degrees
         */
		inline double getAoA() const { return _aoa; }

		/**
         * @return Mach number of simulation
         */
		inline double getMach() const { return _mach; }

		/**
         * @return Velocity in m/s
         */
		inline double getVelocity() const { return _velocity; }

		/**
         * @return  pressure in Pa
         */
		inline double getPressure() const { return _pressure; }

		/**
         * @return  Temperature in K
         */
		inline double getTemperature() const { return _temp; }

		/**
         * @return  Viscosity in Ns/m^2
         */
		inline double getViscosity() const { return _visc; }

		/**
         * @return  Density in kg/m^3
         */
		inline double getDensity() const { return _density; }

		inline double getGamma() const { return _gamma; }

		/**
         * @return Specific heat in J/Kg.k
         */
		inline double getSpecificHeat() const { return _specificHeat; }

		/**
         * @return  Gas constant in J/kg.K
         */
		inline double getGasConstant() const { return _gasConstant; }

		inline double getCFL() const { return _cfl; }

		inline double getMinResidual() const { return _minResidual; }

		inline double getMinAeroCoeffError() const { return _minAeroCoeffError; }

		/**
         * @return  Type of Solver Scheme (ROE or AUSM)
         */
		inline SolverScheme getSolverScheme() const { return _solverScheme; }

		/**
         * @return  Type of Temporal Scheme (EXPLICIT_EULER , IMPLICIT_EULER or RK5)
         */
		inline TemporalScheme getTemporalScheme() const { return _temporalScheme; }

		inline bool getResidualSmoothing() const { return _residualSmoothing; }

		inline int getMaxNumberIterations() const { return _maxIter; }

		inline int getNumberPartitions() const { return _nbPartition; }

		inline std::string getInitialMeshFile() const { return _initialMeshFile; }

		inline std::string getPreLog() const { return _preProcessorLog; }

		inline std::string getSolverLog() const { return _solverLog; }

		inline std::string getPostLog() const { return _postProcessorLog; }

		inline std::pair<int, int> getMeshOrientationCL() const { return _meshOrientationCL; }
		inline std::pair<int, int> getMeshOrientationCD() const { return _meshOrientationCD; }
		inline std::pair<int, int> getMeshOrientationCM() const { return _meshOrientationCM; }
		inline E3D::Vector3<double> getMeshRefPoint() const { return _meshRefPoint; }
		inline int getSamplingPeriod() const { return _samplingPeriod; }
		inline const std::filesystem::path &GetoutputDir() const { return _outputDir; }
		inline double getCref() const { return _Cref; }
		inline double getSref() const { return _Sref; }

	private:
		std::ifstream _configFileStream;
		std::string _initialMeshFile;
		std::vector<std::string> _meshFiles;
		std::string _tecplotFile;
		std::string _configFile;
		std::string _preProcessorLog;
		std::string _solverLog;
		std::string _postProcessorLog;
		std::filesystem::path _outputDir;

		double _Sref;
		double _Cref;
		double _aoa;
		double _mach;
		double _velocity;
		double _pressure;
		double _temp;
		double _visc;
		double _density;
		double _gamma;
		double _specificHeat;
		double _gasConstant;
		double _cfl;
		double _minResidual;
		double _minAeroCoeffError;
		int _samplingPeriod;
		TemporalScheme _temporalScheme;
		SolverScheme _solverScheme;
		int schemeChoice;
		SpeedType _spdoption;
		int speedChoice;
		int _maxIter;
		int _nbPartition;
		// first element is the axis 0=x, 1=y, 2=z
		// second element is the sign 1 or -1
		std::pair<int, int> _meshOrientationCL;
		std::pair<int, int> _meshOrientationCD;
		std::pair<int, int> _meshOrientationCM;
		E3D::Vector3<double> _meshRefPoint;

		bool _residualSmoothing;
	};

}// namespace E3D::Parser