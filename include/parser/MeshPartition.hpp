//
// Created by amin on 2/5/21.
//

#pragma once

#include "parser/SU2MeshParser.hpp"
#include "parser/GhostCell.hpp"

namespace E3D::Parser {
    class MeshPartition : public SU2MeshParser {
    public:
        MeshPartition(const std::string &fileName);

        void parseMpiBoundaryElement();

        void printAllPartitionsInfo(const int rankID);

    private:
        std::vector<E3D::Parser::GhostCell> _MpiBoundaryElements;
        int _nbAdjacentPartitions;
        int _nbMpiBoundaryElements();
    };


}



