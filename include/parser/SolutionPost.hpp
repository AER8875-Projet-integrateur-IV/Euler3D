#pragma once


#include <fstream>
#include <string>
#include <vector>


namespace E3D::Parser {

	/**
     * @author Aziz et Vincentt
     * @brief Parser for solution
     */

	class SolutionPost {

	public:
		/**
         * @brief Default ctor, Executes private member functions to parse data from mesh file.
         */
		SolutionPost(std::string fileName, int nElem);
		~SolutionPost();

		// Getters

		/**
         *
         * @return NUumber of Volume Elements
         */
		inline double GetRho(int iElem) const { return this->_rho[iElem]; }
		inline double GetU(int iElem) const { return this->_u[iElem]; }
		inline double GetV(int iElem) const { return this->_v[iElem]; }
		inline double GetW(int iElem) const { return this->_w[iElem]; }
		inline double GetPression(int iElem) const { return this->_p[iElem]; }
		inline double GetEnergy(int iElem) const { return this->_e[iElem]; }

		inline const std::vector<double> &GetRho() const { return this->_rho; }
		inline const std::vector<double> &GetU() const { return this->_u; }
		inline const std::vector<double> &GetV() const { return this->_v; }
		inline const std::vector<double> &GetW() const { return this->_w; }
		inline const std::vector<double> &GetPression() const { return this->_p; }
		inline const std::vector<double> &GetEnergy() const { return this->_e; }

	private:
		std::string _filename;      /** @brief SU2 mesh file path and name */
		std::ifstream _ifilestream; /** @brief Input file stream */
		int _nElem = 0;             /** @brief Total number of Elements */
		std::vector<double> _rho;   /** @brief density */
		std::vector<double> _u;     /** @brief density */
		std::vector<double> _v;     /** @brief density */
		std::vector<double> _w;     /** @brief density */
		std::vector<double> _p;     /** @brief density */
		std::vector<double> _e;     /** @brief density */

		void parseFile();
	};
}// namespace E3D::Parser
