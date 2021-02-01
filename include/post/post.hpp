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
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "parser/Element.hpp"
// #include "mesh/Mesh.hpp"

namespace E3D::Post
{

    class Post
    {
    private:
        /**
         * Écrit la solution dans un fichier Tecplot
         *
         * @param[in]   fileName   Nom du fichier Tecplot
         */
        void WriteTecplot(std::string fileName);

    public:
        /**
         * @brief       Constructor
         *
         * @param[in]   meshGlobal      mesh to be partitionned with solved connectivity
         * @param[in]   nPart           number of mesh partitions required
         *
         */
        // Post(Mesh *meshGlobal, int &nPart);
        /**
         * Destructeur de la classe
         *
         */
        ~Post();

        void Write();
    };
}; // namespace E3D::Post
