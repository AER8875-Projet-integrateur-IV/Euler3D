/**
 * @file SU2Writer.hpp
 * @author Abraham Bhérer-Constant (abraham.bconstant@gmail.com)
 * @brief Write a mesh object to a file in modified SU2 format
 * @date 2021-01-16
 * 
 * 
 */
#pragma once
#include <fstream>
#include "mesh/Mesh.hpp"
#include "partition/Partition.hpp"

namespace E3D::Partition
{    
    const int CONNEC_ELEMENT2NODE = 0;
    const int CONNEC_MARKER = 1;

    class SU2Writer
    {
    private:
        std::ofstream _m_file;
        int _m_doublePrecision;

    public:
        /**
         * @brief Construct a new SU2Writer object
         * 
         * @param path path to the output file
         * @param doublePrecision number of digits to be used for writing floats
         */
		SU2Writer(const std::string &path, int doublePrecision = 10);

		/**
         * @brief Destroy the SU2Writer object
         * 
         */
        ~SU2Writer();

        /**
         * @brief Write the whole mesh to file
         * 
         * @param mesh mesh object to be written to file
         */
        void Write(std::vector<E3D::Parser::Element>& interiorElemVector,
                    int nDim, std::vector<E3D::Parser::Node>& nodeVector,
                    E3D::Parser::BC_Structure &bc);

        /**
         * @brief Write the volume elements 2 node connectivity to file
         * 
         * @param elemVector Vector of elements from which the connectivity data
         *                  will be extracted 
         */
        void WriteElement2Node(std::vector<E3D::Parser::Element>& elemVector);

        /**
         * @brief Write element 2 node connectivity for a vector of elements
         * 
         * @param elemVector Vector of elements from which the connectivity data
         *                  will be extracted 
         * @param type Define if the connectivity being written is for volume
         *                  elements or for marker elements. Possible options
         *                  include E3D::Partition::CONNEC_ELEMENT2NODE or
         *                  E3D::Partition::CONNEC_MARKER
         */
        void WriteConnec(const std::vector<E3D::Parser::Element>& elemVector, int type);

        /**
         * @brief Write the coordinates of all the node in the input mesh object
         * 
         * @param nDim Number of dimensions in the mesh
         * @param nodeVector Node vector containing the coordinates
         */
		void WriteCoord(std::vector<E3D::Parser::Node> &nodeVector);

		/**
         * @brief Write the standard markers (farfield and wall) to file
         * 
         * @param markers Border condition data as defined in SU2MeshParser.hpp
         */
        void WriteMarker(E3D::Parser::BC_Structure &markers);

        /**
         * @brief Write to a file the inter-partition connectivity for the internal 
         *        Border Conditions
         * @details Write the inter-partition connectivity in a non SU2 format,
         *          This allows the reconstructions of internal borders for multi-
         *          threading with OpenMPI. 
         *  NINTERNAL_MARKER= {number of adjacent partitions}
         *  INTERNAL_MARKER= {Adjacent partition ID}
         *  % local element ID (this partition)    local element ID (other partition)
         *  1       2
         *  2       54
         *  3       34
         */ 
        void WriteInternalMarker();
    };
}
