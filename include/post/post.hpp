/**
 * @file Post.hpp
 * @author Vincent Bar, Aziz Ndiaye, Abraham Bherer-Constant
 * @date 2021/02/01
 * @brief Write a solution file to read on Tecplot
 *
 * Wrtie a solution file. Write a mesh on which connectivity has already been solved with
 * the METIS library into multiple smaller mesh and write as well solution for other parameters
 * such as Pressure, Density, Velocity and Energy already solved in Solver.
 */
#pragma once
#include "/usr/local/tecplot/360ex_2019r1/include/TECIO.h"
#include <iostream>
#include <string.h>
#include <unordered_map>
#include <vector>

#include "parser/Element.hpp"
#include "parser/SU2MeshParser.hpp"
#include "parser/SimConfig.hpp"
#include "parser/SolutionPost.hpp"

namespace E3D::Post {

	class Post {
	private:
		/*Fichier maillage*/
		std::vector<std::string> _meshPartitionPath;
		/*Fichiers variable solveur*/
		std::vector<std::string> _solutionPartitionPath;
		// Fichier de sortie
		std::string _outputFile;
		/*Nombre de partitions*/
		int _nPart;

		/**
         * Écrit la solution dans un fichier Tecplot
         *
         * @param[in]   fileName   Nom du fichier Tecplot
         */
		void WriteTecplotASCII();
		void WriteTecplotBinary();

	public:
		Post(const E3D::Parser::SimConfig &config);
		~Post();

		void Write();
	};
};// namespace E3D::Post
