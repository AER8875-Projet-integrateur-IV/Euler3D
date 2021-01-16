

#include "partition/SU2Writer.hpp"

#include <string>
#include <ofstream>
#include <vector>
#include <algorithm>

#include "parser/Element.hpp"
#include "parser/SU2MeshParser.hpp"


SU2Writer::SU2Writer(std::string path, Mesh* mesh)
{
    _m_file = std::ofstream(path);
    _m_mesh = mesh;
}

SU2Writer::~SU2Writer()
{
    _m_file.close()
}

void SU2Writer::Write(){
    this->WriteCoord();
    this->WriteElement2Node();
    this->WriteMarker();

}

void SU2Writer::WriteElement2Node(){
    int nElem = mesh->getNElem();
    std::vector<E3D::Parser::Element> elemVector mesh->GetVolumeElems();
    _m_file << "NELEM= " << nElem << "\n\n";
    this->WriteConnec(elemVector);
}

void SU2Writer::WriteConnec(std::vector<E3D::Parser::Element> &elemVector){
    for(E3D::Parser::Element const &elem : *elemVector){
        std::vector<int> nodes = elem->getElemNodes();
        for(int const &node : nodes){
            _m_file << *node << " ";
        }
        _m_file << "\n";
    }

}

void SU2Writer::WriteCoord(){
    int nNode = mesh->getNNode();
    int nDime = mesh->getNDime();
    double val;

    _m_file << "NPOIN= " << nNode << "\n\n";
    for (int nodeI = 0; nodeI < nNode; nodeI++)
    {
        for (int dimI = 0; dimI < nDime; dimI++)
        {
            val = mesh->getCoord(nodeI+dimI);
            _m_file << val;
        }
        
        _m_file << "\n";
    }
    _m_file << "\n"
    
}

void SU2Writer::WriteMarker(){

    int nMarkers = _m_mesh->GetMarkersCount();
    int nElem;    
    std::string *tag;
    std::vector<E3D::Parser::BC_Structure> markers = _m_mesh->GetBoundaryElems();

    _m_file << "NMARK= " << nMarkers << "\n\n";
    for (E3D::Parser::BC_Structure const &marker : markers){
        tag = marker->first;
        nElem = marker->second.size();
        _m_file << "MARKER_TAG= " << *tag << "\n";
        _m_file << "MARKER_ELEMS= " << nElem << "\n";
        this->WriteConnec(marker->second);
    }
}

void SU2Writer::WriteInternalMarker(){
    
}