//
// Created by amin on 2/5/21.
//

#pragma once

#include "parser/SU2MeshParser.hpp"

namespace E3D::Parser {
    class SU2PartitionParser : public SU2MeshParser {
    public:
        SU2PartitionParser(const std::string &fileName, const int rankID);
    };
}



