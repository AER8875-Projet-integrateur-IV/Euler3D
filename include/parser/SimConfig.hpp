#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace E3D::Parser {


	class SimConfig {
	public:
		SimConfig(const std::string &filename, const int rankID, const int poolSize);
		SimConfig(const std::string &filename);

		void parseConfigFile();

		void printInfo();

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

		/**
         * @return  Type of Solver Scheme (ROE or AUSM)
         */
		inline SolverScheme getSolverScheme() const { return _solverScheme; }

		/**
         * @return  Type of Temporal Scheme (EXPLICIT_EULER , IMPLICIT_EULER or RK5)
         */
		inline TemporalScheme getTemporalScheme() const { return _temporalScheme; }

		inline int getMaxNumberIterations() const { return _maxIter; }

		inline int getNumberPartitions() const { return _nbPartition; }

		inline std::string getInitialMeshFile() const { return _initialMeshFile; }

		inline std::string getPreLog() const { return _preProcessorLog; }

		inline std::string getSolverLog() const { return _solverLog; }

		inline std::string getPostLog() const { return _postProcessorLog; }

	private:
		std::ifstream _configFileStream;
		std::string _initialMeshFile;
		std::vector<std::string> _meshFiles;
		std::string _tecplotFile;
		std::string _configFile;
		std::string _preProcessorLog;
		std::string _solverLog;
		std::string _postProcessorLog;
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
		TemporalScheme _temporalScheme;
		SolverScheme _solverScheme;
		SpeedType _spdoption;
		int _maxIter;
		int _nbPartition;
	};

}// namespace E3D::Parser