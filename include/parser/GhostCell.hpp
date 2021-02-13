#pragma once


namespace E3D::Parser {
    struct GhostCell {

        GhostCell(int thisParElemID, int OtherParElemID, int faceVtkID, std::vector<int> faceNodes)
                : _faceNodeID(faceNodes),
                  _faceVtkID(faceVtkID),
                  _thisPartitionElementID(thisParElemID),
                  _adjacentPartitionElementID(OtherParElemID) {};

    private:

        std::vector<int> _faceNodeID;           /** Ids of nodes forming the face between 2 elements */
        int _faceVtkID;                         /** VTK id of the face between the 2 elements */
        int _thisPartitionElementID;            /** ID of the element in the partition of this MPI process */
        int _adjacentPartitionElementID;        /** ID of the element in neighbor partition */

    };
}