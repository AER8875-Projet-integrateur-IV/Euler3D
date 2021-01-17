#pragma once
#include "parser/Element.hpp"
#include "parser/SU2MeshParser.hpp"

struct SU2Mesh
{
    //variables lues
    int nDim;
    int nElem;
    int nPoin;
    std::vector<E3D::Parser::Element> elem2node;
    std::vector<int> interfaceStart; // entre les partitions, a voir
    std::vector<int> interface;
    E3D::Parser::BC_Structure Boundary;
    std::vector<E3D::Parser::Node> coord;

    // variable calculees et assignees par connectivity
    int nFace;
    int nElemTot;
    std::vector<int> node2elem;
    std::vector<int> node2elemStart;
    std::vector<int> elem2elem;
    std::vector<int> elem2elemStart;
    std::vector<int> elem2face;
    std::vector<int> elem2faceStart;
    std::vector<int> face2elem;
    std::vector<int> face2elemStart;

    //a confirmer, implementer des methodes pour simplifier l'indexation
    inline const uint32_t &ElemToNode(const uint32_t &ElemId, const uint32_t &LocalNodeID) const {
			return 0;
		}
};
