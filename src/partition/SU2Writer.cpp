#include "partition/SU2Writer.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "partition/Partition.hpp"
#include "parser/Element.hpp"
#include "mesh/Mesh.hpp"

#include <iomanip> 

using namespace E3D::Partition;

SU2Writer::SU2Writer(std::string path, int doublePrecision)
{
    _m_file = std::ofstream(path);
    _m_doublePrecision = doublePrecision;
    _m_file << std::setprecision(_m_doublePrecision) 
            << std::setiosflags(std::ios::dec)
            << std::setiosflags(std::ios::scientific);
}

SU2Writer::~SU2Writer()
{
    _m_file.close();
}

void SU2Writer::Write(std::vector<E3D::Parser::Element>& interiorElemVector,
                        int nDim, std::vector<E3D::Parser::Node>& nodeVector,
                        E3D::Parser::BC_Structure &bc){
    this->WriteElement2Node(interiorElemVector);
    this->WriteCoord(nDim, nodeVector);
    this->WriteMarker(bc);
}

void SU2Writer::WriteElement2Node(std::vector<E3D::Parser::Element>& elemVector){
    int nElem = elemVector.size();
    _m_file << "NELEM= " << nElem << "\n\n";
    this->WriteConnec(elemVector);
}

void SU2Writer::WriteConnec(const std::vector<E3D::Parser::Element>& elemVector){
    std::vector<int> nodes;
    for (E3D::Parser::Element const &elem : elemVector)
    {
        _m_file << elem.getVtkID();
        nodes = elem.getElemNodes();
        for(int const &node : nodes){
            _m_file << " " << node;
        }
        _m_file << std::endl;        
    }
}


void SU2Writer::WriteCoord(int nDim, std::vector<E3D::Parser::Node>& nodeVector){
    double val;
    int nNode = nodeVector.size();
    _m_file << "NPOIN= " << nNode << "\n\n";
    for (E3D::Parser::Node const &node : nodeVector)
    {
        _m_file << node.getX() << " " << node.getY() << " " << node.getZ() << std::endl;
    }
    _m_file << std::endl;
    
}

void SU2Writer::WriteMarker(E3D::Parser::BC_Structure &markers){

    int nMarkers = markers.size();
    int nElem;    
    std::string tag;

    _m_file << "NMARK= " << nMarkers << "\n";
    for (std::pair<std::string, std::vector<E3D::Parser::Element>> const &marker : markers){
        tag = marker.first;
        nElem = marker.second.size();
        _m_file << "MARKER_TAG= " << tag << "\n";
        _m_file << "MARKER_ELEMS= " << nElem << "\n";
        this->WriteConnec(marker.second);
    }
}

void SU2Writer::WriteInternalMarker(){
    
}