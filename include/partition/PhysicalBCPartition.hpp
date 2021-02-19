/**
 * @file PhysicalBCPartition.hpp
 * @author Abraham Bherer-Constant
 * @date 2021/02/19  
 *
 */

#pragma once

#include <vector>
#include "partition/Partition.hpp"

namespace E3D::Partition
{
    /**
     * @brief Partition the physical partitions between subpartitions
     * 
     */
    class PhysicalBCPartition
    {
    public:
        /**
         * @brief Partition the physical border conditions to their respective
         * subpartition
         * 
         * @param globalBC border conditions to be partitionned from the global
         *      mesh
         * @param parts vector of subpartitions after processing by METIS
         */
        static void Solve(const E3D::Parser::BC_Structure &globalBC,
                                std::vector<SU2Mesh> &parts);   

		/**
         * @brief Check all partition element for a match with a marker from the
         *      global mesh
         * 
         * @param partID partition ID to check for match
         * @param markerNodes Global node IDs from the marker element
         * @param localMarkerNodes Will be overwritten with the local node IDs
         *      from the partition matching the markerNodes vector
         */
        static void FindMarkerInPartition(E3D::Partition::SU2Mesh &part, 
                                   const std::vector<int> &markerNodes, 
                                   std::vector<int>& localMarkerNodes); 

        /**
         * @brief Check if all values in subSet can be found in globalSet
         * 
         * @details Check if all values in subset are found in global. If all 
         *      values are found, indexVector will be overwritten with the
         *      index of the matches found in globalSet. If not all values are
         *      found, indexVector will be overwritten with an empty vector.
         * @param subSet vector of values to be looken for in globalSet must be
         *      smaller or equal size to globalSet
         * @param globalSet vector of values that will be used to extract the 
         *      index of matches
         * @param indexVector Stores the index of matches found if all values in
         *      subSet are found in globalSet
         */
        static void FindContainedElements(const std::vector<int>& subSet, 
                                          const std::vector<int>& globalSet, 
                                          std::vector<int>& indexVector);
    };
}