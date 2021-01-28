#pragma once

#include <string>
#include <sstream>
namespace E3D::Parser {


    class ConfigParser {
    public :
        ConfigParser(std::string filename);

        /**
         *
         * @param ifilestream input file stream from config file
         */

        void parseConfigFile(std::ifstream& ifilestream);

        /**
         * @brief Getters of angle of attack
         * @return Angle of attack in degrees
         */
        inline double GetAoA() {return _aoa;}

        /**
         *
         * @return
         */
        inline double GetMach() { return _mach;}


    private:
        int _processCount; /** Holds number of MPI processes for this sim run */
        double _aoa;          /** Angle of attack in degrees */
        double _mach;         /** Mach number */

    };

} // namespace E3D::Parser