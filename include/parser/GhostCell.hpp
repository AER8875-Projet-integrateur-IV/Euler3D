#pragma once

#include <ostream>
#include <vector>

namespace E3D::Parser {
	struct GhostCell {

		GhostCell(int thisParElemID, int OtherParElemID, int faceVtkID, std::vector<int> &faceNodes)
		    : _faceNodeIDs(faceNodes),
		      _faceVtkID(faceVtkID),
		      _thisPartitionElementID(thisParElemID),
		      _adjacentPartitionElementID(OtherParElemID){};

		friend std::ostream &operator<<(std::ostream &out, const GhostCell &cell) {

			out << " GhostCell ID : " << cell.getGhostCellID()
			    << " | This Parition Element ID: " << cell.getthisPartitionElementID()
			    << " | Adjacent Parition Element ID: " << cell.getAdjacentPartitionElementID()
			    << " | Face VTK ID : " << cell.getFaceVtkID()
			    << " | Face Nodes : ";
			for (auto &nodeID : cell.getfaceNodeIDs()) {
				out << nodeID << " ";
			}
			return out;
		}

		//Todo add < operator


		inline int getthisPartitionElementID() const { return _thisPartitionElementID; }

		inline int getAdjacentPartitionElementID() const { return _adjacentPartitionElementID; }

		inline const std::vector<int> &getfaceNodeIDs() const { return this->_faceNodeIDs; }

		inline int getFaceVtkID() const { return _faceVtkID; }

		inline void setGhostCellID(int ID) { _ghostCellID = ID; }

		inline int getGhostCellID() const { return _ghostCellID; }

	private:
		std::vector<int> _faceNodeIDs;   /** Ids of nodes forming the face between 2 elements */
		int _faceVtkID;                  /** VTK id of the face between the 2 elements */
		int _thisPartitionElementID;     /** ID of the element in the partition of this MPI process */
		int _adjacentPartitionElementID; /** ID of the element in neighbor partition */
		int _ghostCellID;
	};
}// namespace E3D::Parser