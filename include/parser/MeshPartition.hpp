//
// Created by amin on 2/5/21.
//

#pragma once

#include "parser/SU2MeshParser.hpp"
#include "parser/GhostCell.hpp"


namespace E3D::Parser {
    class MeshPartition : public SU2MeshParser {
    public:
        MeshPartition(const std::string &fileName, const int);

        void printAllPartitionsInfo();

        inline std::vector<std::pair<int, std::vector<E3D::Parser::GhostCell>>> &getMpiBoundaryElems() {
            return this->_MpiBoundaryElements;
        }

        inline const std::vector<int> &getAdjacentPartitionsID() const { return _adjacentPartitionsID; }

        inline int getMpiBoundaryElemsCount() const { return _nbMpiBoundaryElements; }

        inline int getAdjacentPartitionsCount() const { return _nbAdjacentPartitions; }

        inline int getrankID() const {return _rankID;}

    private:
        /**
         * @ Parse MPI BC elements to hold in _MpiBoundaryElements member variable
         */
        void parseMpiBoundaryElement();


        std::vector<std::pair<int, std::vector<E3D::Parser::GhostCell>>> _MpiBoundaryElements; /** vector Holding pairs of Partition ID
                                                                                                 *  and ghost cells in that partition
                                                                                                 */
        std::vector<int> _adjacentPartitionsID;
        int _nbAdjacentPartitions;
        int _nbMpiBoundaryElements = 0;
		int _rankID;

    };


}



