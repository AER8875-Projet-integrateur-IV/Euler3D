#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

namespace E3D::Parser {


    class SimConfig {
    public :
        SimConfig(std::string filename);

        void parseConfigFile();

         enum class SpeedType{
            MACH,
            METER_PER_SEC
        };

        enum class SolverScheme{
            ROE,
            AUSM
        };

        enum class TemporalScheme{
            EXPLICIT_EULER,
            RK5,
            IMPLICIT_EULER
        };

    public:



        [[nodiscard]] inline const std::vector<std::string>& getPartitionedMeshFiles() const { return _meshFiles; }

        [[nodiscard]] inline const std::string& getTecplotFile() const                       { return _tecplotFile; }

        inline double getAoA() const                                                         { return _aoa; }

        inline double getMach() const                                                        { return _mach; }

        inline double getVelocity() const                                                    { return _velocity;}

        inline double getPressure() const                                                    { return _pressure; }

        inline double getTemperature() const                                                 { return _temp;}

        inline double getViscosity() const                                                   { return _visc; }

        inline double getDensity() const                                                     { return _density; }

        inline double getGamma() const                                                       { return _gamma; }

        inline double getSpecificHeat() const                                                { return _specificHeat; }

        inline double getGasConstant() const                                                 { return _gasConstant; }

        inline double getCFL() const                                                         { return _cfl; }

        inline double getMinResidual() const                                                 { return _minResidual; }

        inline SolverScheme getSolverScheme() const                                          { return _solverScheme; }

        inline TemporalScheme getTemporalScheme() const                                      { return _temporalScheme;}

        inline int getMaxNumberIterations() const                                            { return _maxIter; }

        inline int getNumberPartitions() const                                               { return _nbPartition; }


    private:


        std::ifstream _configFileStream;
        std::vector<std::string> _meshFiles;
        std::string _tecplotFile;
        std::string _configFile;
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

} // namespace E3D::Parser