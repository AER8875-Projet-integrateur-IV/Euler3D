#include "partition/SU2Writer.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "partition/Partition.hpp"
#include "parser/Element.hpp"
#include "mesh/Mesh.hpp"
#include <boost/range/adaptor/indexed.hpp>
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
    _m_file << "%\n"
            << "% Problem dimension\n"
            << "%\n"
            << "NDIME= " << nDim << "\n";
    this->WriteElement2Node(interiorElemVector);
    this->WriteCoord(nDim, nodeVector);
    this->WriteMarker(bc);
}

void SU2Writer::WriteElement2Node(std::vector<E3D::Parser::Element>& elemVector){
    int nElem = elemVector.size();
    _m_file << "%\n"
            << "% Inner element connectivity\n"
            << "%\n";
    _m_file << "NELEM= " << nElem << "\n";
    this->WriteConnec(elemVector, CONNEC_ELEMENT2NODE);
}

void SU2Writer::WriteConnec(const std::vector<E3D::Parser::Element>& elemVector, int type){
    std::vector<int> nodes;
    for (const auto &elem : elemVector | boost::adaptors::indexed())
    {
        _m_file << std::setw(3) << elem.value().getVtkID();
        nodes = elem.value().getElemNodes();
        for(int const &node : nodes){
            _m_file << std::setw(10) << node; 
        }
        if(type==CONNEC_ELEMENT2NODE){
            _m_file << std::setw(10) << elem.index(); 
        }
        _m_file << std::endl;        
    }
}


void SU2Writer::WriteCoord(int nDim, std::vector<E3D::Parser::Node>& nodeVector){
    int nNode = nodeVector.size();
    _m_file << "%\n"
            << "% Node coordinates\n"
            << "%\n";
    _m_file << "NPOIN= " << nNode << "\n";
    
    for (const auto &node : nodeVector | boost::adaptors::indexed())
    {
        _m_file << std::setw(20) << node.value().getX() 
                << std::setw(20) << node.value().getY() 
                << std::setw(20) << node.value().getZ()
                << std::setw(10) << node.index()
                << std::endl;
    }
    
}

void SU2Writer::WriteMarker(E3D::Parser::BC_Structure &markers){
    int nMarkers = markers.size();
    int nElem;    
    std::string tag;
    _m_file << "%\n"
            << "% Boundary elements\n"
            << "%\n";
    _m_file << "NMARK= " << nMarkers << "\n";
    for (std::pair<std::string, std::vector<E3D::Parser::Element>> const &marker : markers){
        tag = marker.first;
        nElem = marker.second.size();
        _m_file << "MARKER_TAG= " << tag << "\n";
        _m_file << "MARKER_ELEMS= " << nElem << "\n";
        this->WriteConnec(marker.second, CONNEC_MARKER);
    }
}

void SU2Writer::WriteInternalMarker(){
    
}