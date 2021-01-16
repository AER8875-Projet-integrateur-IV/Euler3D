/**
 * @file SU2Writer.hpp
 * @author Abraham Bhérer-Constant (abraham.bconstant@gmail.com)
 * @brief Write a mesh object to a file in modified SU2 format
 * @date 2021-01-16
 * 
 * 
 */
#pragma once
#include <ofstream>

class Mesh{};

class SU2Writer
{
private:
    std::ofstream _m_file;
    Mesh* _m_mesh;
public:
    /**
     * @brief Construct a new SU2Writer object
     * 
     * @param path path to the output file
     * @param mesh mesh object to be written to file
     */
    SU2Writer(std::string path, Mesh* mesh);

    /**
     * @brief Destroy the SU2Writer object
     * 
     */
    ~SU2Writer();

    /**
     * @brief Write the whole mesh to file
     * 
     */
    void Write();

    /**
     * @brief Write the volume elements 2 node connectivity to file
     * 
     */
    void WriteElement2Node();

    /**
     * @brief Write the element 2 node connectivity of a vector of elements
     * 
     * @param elemVector Elements to be written to file. The elements can be
     *                  volumique or border elements
     */
    void WriteConnec(std::vector<E3D::Parser::Element> &elemVector);

    /**
     * @brief Write the coordinates of all the node in the input mesh object
     * 
     */
    void WriteCoord();

    /**
     * @brief Write the standard markers (farfield and wall) to file
     * 
     */
    void WriteMarker();

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
    void WriteInternalMarker()
};

