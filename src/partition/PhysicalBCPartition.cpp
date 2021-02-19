#include "partition/PhysicalBCPartition.hpp"
#include "partition/Partition.hpp"
#include "mesh/Mesh.hpp"
#include <vector>
#include <algorithm>   

using namespace E3D::Partition;

void PhysicalBCPartition::Solve(const E3D::Parser::BC_Structure &globalBC,
                                std::vector<SU2Mesh> &parts){
    std::vector<int> localMarkerNodes;
    for(auto const &Marker : globalBC){
        const std::string &tag = Marker.first;
        const std::vector<E3D::Parser::Element> &elemVector = Marker.second;
        
        
        // Find a match for each border element of the global mesh
        for(auto const &elem : elemVector){
            const std::vector<int> &markerNodes = elem.getElemNodes();
            int failedMatch = 0;
            // Look for a match in each partition
            for (auto &part : parts)
            {
                PhysicalBCPartition::FindMarkerInPartition(part, markerNodes, localMarkerNodes);
                if(localMarkerNodes.empty()){
                    // No match has been found in this partition
                    failedMatch++;
                } else{
                    // A match has been found
                    int VTKid = elem.getVtkID();
                    part.AddMarkerElement(tag, VTKid, localMarkerNodes.data(), localMarkerNodes.size());
                    break;
                }
            }
            if(failedMatch == static_cast<int>(parts.size())){
                throw std::runtime_error("Marker could not be found in a subpartition");
            }
        }
    }
}

void PhysicalBCPartition::FindMarkerInPartition(SU2Mesh &part,
                                            const std::vector<int> &markerNodes, 
                                            std::vector<int>& localMarkerNodes){
    // This function could be optimised if needed

    localMarkerNodes = std::vector<int>();
    localMarkerNodes.reserve(markerNodes.size());

    // Go over every element in the partition, if all nodes in
    // markerNodes are found in a element, elem is part of the 
    // current partition
    for (int elemI = 0; elemI < part.NELEM; elemI++)
    {
        int start = part.elem2nodeStart[elemI];
        int end = part.elem2nodeStart[elemI+1];
        int size = end-start;
        std::vector<int> globalPartNode;
        globalPartNode.reserve(size);
        std::vector<int> localPartNode;
        localPartNode.reserve(size);
        
        // Build local node vector
        for (int i = start; i < end; i++)
        {
            localPartNode.push_back(part.elem2node[i]);
        }
        // Build global node vector
        for (int localID : localPartNode)
        {     
            globalPartNode.push_back(part.LocalNode2global(localID));
        }
        
        std::vector<int> matchIndex;
        FindContainedElements(markerNodes, globalPartNode, matchIndex);
        
        if (matchIndex.size() == markerNodes.size())
        {
            for(int index : matchIndex){
                localMarkerNodes.push_back(localPartNode[index]);
            }

            // a match has been found
            return;
        }
    }
    // No match has been found
}

void PhysicalBCPartition::FindContainedElements(const std::vector<int>& subSet, 
                                                const std::vector<int>& globalSet, 
                                                std::vector<int>& indexVector){
    const int *findStart = globalSet.data();
    const int *findEnd = findStart+globalSet.size();

    // This vector contains the index (taken from globalSet) of matching values
    indexVector = std::vector<int>();
    indexVector.reserve(subSet.size());

    for(const int &value : subSet){
        const int* p = std::find(findStart, findEnd, value);
        if (p == findEnd){
            // value not found in globalSet
            break;
        } else{
            // value is part of globalSet
            // save the index
            indexVector.push_back(p-findStart);
        }        
    }
    // All elements were found    
}