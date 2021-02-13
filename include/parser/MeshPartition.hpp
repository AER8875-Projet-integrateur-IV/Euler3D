//
// Created by amin on 2/5/21.
//

#pragma once

#include "parser/SU2MeshParser.hpp"

namespace E3D::Parser {
    class MeshPartition : public SU2MeshParser {
    public:
        MeshPartition(const std::string &fileName, const int rankID);
    };
}



