#pragma once
#include <algorithm>
#include <memory>
#include <mpi.h>
#include <string>
#include <type_traits>

#include "connectivity/Connectivity.hpp"
#include "parser/Element.hpp"
#include "parser/MeshPartition.hpp"
#include "parser/SU2MeshParser.hpp"


namespace E3D {


	/**
	 *
	 * @tparam T type of the mesh : either SU2MeshParser or MeshPartition
	 */
	template<typename T>
	class Mesh {
	public:
		/**
		 * @brief ctor for preprecossor without MPI, use this ctor with type SU2MeshParser
		 * @param fileName
		 */
		Mesh(const std::string &fileName) : _parser(fileName) {
			_parser.printInfo();
		};

		Mesh(const std::string &fileName, const int RankID) : _parser(fileName, RankID) {
			_parser.printAllPartitionsInfo();
		}


		void solveConnectivity() {
			// IF type of the parser == MeshPartition, print info
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {

				if (_parser.getrankID() == 0) {
					std::cout << "\n\n"
					          << std::string(24, '#') << "  Connectivity  " << std::string(24, '#') << "\n\n";
				}

				double startConnectivityTimer = MPI_Wtime();
			}


			// Call connectivity methods to solve and populate connectivity vectors (Remplir ici) :
			connectivityObj._nNode = GetMeshNodeCount();
			connectivityObj.SolveElement2node(GetInteriorElementVector());

			connectivityObj.ComputeVTKLinkedLists(GetInteriorVTKID());
			connectivityObj.SolveNode2element();
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				int NelemMPI = _parser.getMpiBoundaryElemsCount();
				connectivityObj.SolveElement2Element(GetInteriorVTKID(), GetMeshBoundaryElemCount() + NelemMPI);
			} else {
				connectivityObj.SolveElement2Element(GetInteriorVTKID(), GetMeshBoundaryElemCount());
			}


			// Filling private member variables related to connectivity
			node2element = std::make_unique<std::vector<int>>(connectivityObj._node2element);
			node2elementStart = std::make_unique<std::vector<int>>(connectivityObj._node2elementStart);
			element2element = std::make_unique<std::vector<int>>(connectivityObj._element2element);
			element2elementStart = std::make_unique<std::vector<int>>(connectivityObj._element2elementStart);
			element2face = std::make_unique<std::vector<int>>(connectivityObj._element2face);
			element2faceStart = std::make_unique<std::vector<int>>(connectivityObj._element2faceStart);
			face2element = std::make_unique<std::vector<int>>(connectivityObj._face2element);
			face2node = std::make_unique<std::vector<int>>(connectivityObj._face2node);
			face2nodeStart = std::make_unique<std::vector<int>>(connectivityObj._face2nodeStart);


			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {

				// Call private functions to update ghost cells
				updateMPIGhostCells();
				updateWallGhostCell();
				updateFarfieldGhostCell();
				updateSymmetryGhostCell();


				int NghostCellsFarfield = FarfieldGhostCellIDs.size();
				int NghostCellsSymmetry = SymmetryGhostCellIDs.size();
				int NghostCellsWall = WallGhostCellIDs.size();
				int NghostCellsMpi = GetMpiElemsCount();

				std::array<int, 4> GhostCellStats{NghostCellsFarfield, NghostCellsWall, NghostCellsSymmetry, NghostCellsMpi};
				std::array<int, 4> sumGhostCells{0, 0, 0, 0};
				MPI_Reduce(&GhostCellStats, &sumGhostCells, 4, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


				MPI_Barrier(MPI_COMM_WORLD);
				if (_parser.getrankID() == 0) {
					double endConnectivityTimer = MPI_Wtime();
					printf("All processes solved connectivity of partitioned meshes !\n");

					printf("Number of Farfield Ghost Cells : %d\n", sumGhostCells[0]);
					printf("Number of Wall Ghost Cells :     %d\n", sumGhostCells[1]);
					printf("Number of Symmetry Ghost Cells : %d\n", sumGhostCells[2]);
					printf("Number of MPI Ghost Cells :      %d\n\n", sumGhostCells[3]);

					printf("Connectivity took %.5f seconds to solve.\n", endConnectivityTimer - startConnectivityTimer);
				}
			}
		}

		// ------------------ Mesh parsing Info ----------------------

		inline const std::vector<std::pair<int, std::vector<int>>> &GetMPIGhostCellsIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return MPIGhostCellsIDs;
			}
		};


		/**
         * @return IDs of ghost cells associated with a wall BC
         */
		inline const std::vector<int> &GetWallGhostCellsIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return WallGhostCellIDs;
			}
		};

		/**
         * @return IDs of Volume cells connected to a ghost cell
         * associated with a wall BC. They are placed in same order
         * -> WallADjacentToGhostCellIds[i] is connected to WallGhostCellIDs[i]
         */
		inline const std::vector<int> &GetWallAdjacentGhostCellIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return WallAdjacentToGhostCellIDs;
			}
		};


		inline const std::vector<int> &GetWallAdjacentFaceIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return WallAdjacentFaceIDs;
			}
		};

		/**
         * @return IDs of ghost cells associated with a Sym BC
         */
		inline const std::vector<int> &GetSymmetryGhostCellsIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return SymmetryGhostCellIDs;
			}
		};

		/**
         * @return IDs of Volume cells connected to a ghost cell
         * associated with a sym BC. They are placed in same order
         * -> SymmteryAdjacentToGhostCellIds[i] is connected to SymmetryGhostCellIDs[i]
         */
		inline const std::vector<int> &GetSymmetryAdjacentGhostCellIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return SymmetryAdjacentGhostCellIDs;
			}
		};


		inline const std::vector<int> &GetSymmetryAdjacentFaceIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return SymmetryAdjacentFaceIDs;
			}
		};


		/**
         * @return IDs of ghost cells associated with a Farfield BC
         */
		inline const std::vector<int> &GetFarfieldGhostCellsIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return FarfieldGhostCellIDs;
			}
		};

		/**
         * @return IDs of Volume cells connected to a ghost cell
         * associated with a Farfield BC. They are placed in same order
         * -> FarfieldAdjacentToGhostCellIds[i] is connected to FarfieldGhostCellIDs[i]
         */
		inline const std::vector<int> &GetFarfieldAdjacentToGhostCellsIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return FarfieldAdjacentToGhostCellIDs;
			}
		};


		inline const std::vector<int> &GetFarfieldAdjacentFaceIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return FarfieldAdjacentFaceIDs;
			}
		};


		inline int GetMpiElemsCount() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return _parser.getMpiBoundaryElemsCount();

			} else
				return 0;
		}

		inline std::vector<int> getAdjacentPartitionsIDs() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return _parser.getAdjacentPartitionsID();
			}
		}

		inline int getAdjacentPartitionsCount() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return _parser.getAdjacentPartitionsCount();
			}
		}

		inline std::vector<std::pair<int, std::vector<E3D::Parser::GhostCell>>> &getMPIelements() {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return _parser.getMpiBoundaryElems();
			}
		}

		/**
		 * @brief Get MPI rank of this mesh (parition)
		 */
		inline int getMeshRankID() const {
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				return _parser.getrankID();

			}
		}


		inline int GetMeshDim() const { return _parser.GetnDim(); }

		inline int GetMeshInteriorElemCount() const { return _parser.GetVolumeElemCount(); }

		inline int GetMeshBoundaryElemCount() const { return _parser.GetBoundaryElemCount(); }

		inline int GetMeshBConditionCount() const { return _parser.GetMarkersCount(); }

		inline int GetMeshNodeCount() const { return _parser.GetPointsCount(); }


		/**
         * @param NodeID ID of the node
         * @return object Node
         * @example : To get x coordinates of first node : double x = GetNodeCoord[0].getX()
         */

		inline const E3D::Parser::Node GetNodeCoord(const int NodeID) const {
			return _parser.GetPoints()[NodeID];
		}

		/**

         * @return Node vector
         */

		inline const std::vector<E3D::Parser::Node> &GetNodeVector() const {
			return _parser.GetPoints();
		}

		/**
         *
         * @param tagID BC position in mesh file (first [0], second [1] ...)
         * @param ElementID Element position
         * @return Element Object
         * @example To get 3rd element in first BC : Element e = GetBoundaryElement(0,2)
         */

		inline const E3D::Parser::Element GetBoundaryElement(int tagID, int ElementID) const {
			return _parser.GetBoundaryElems()[tagID].second[ElementID];
		}

		/**


         * @brief Get the Boundary Condition Vector object
         *
         * @return const E3D::Parser::BC_Structure
         */
		inline const E3D::Parser::BC_Structure &GetBoundaryConditionVector() const {
			return _parser.GetBoundaryElems();
		}

		/**

         * @param ElementID Element Position
         * @return Element object
         */

		inline const E3D::Parser::Element GetInteriorElement(int ElementID) const {
			return _parser.GetVolumeElems()[ElementID];
		}

		/**
         * @return Interior Element vector
         */

		inline const std::vector<E3D::Parser::Element> &GetInteriorElementVector() const {
			return _parser.GetVolumeElems();
		}

		/**
         * @param tagID BC position in mesh file (first [0], second [1] ...)
         * @return string holding the name of the BC
         */

		inline const std::string GetTagName(int tagID) const {
			return _parser.GetTags()[tagID].first;
		}

		/**

         * @param tagID BC position in mesh file (first [0], second [1] ...)
         * @return int holding the number of element in this BC
         */

		inline int GetNumberOfElementsInTag(int tagID) const {
			return _parser.GetTags()[tagID].second;
		}

		inline const std::vector<int> &GetInteriorVTKID() const {
			return _parser.GetInteriorElementVtkID();
		}

		// ------------------ Connectivity Info ----------------------

		inline int GetnFace() const {
			return connectivityObj._nFace;
		}

		inline int GetnElemTot() const {
			return connectivityObj._nElem;
		}

		/**
         * @brief get element IDs associated to a node ID
         *
         * @param nodeID
         * @param nElem Number of elements arround nodeID
         * @return int*
         */
		inline int *GetNode2ElementID(int nodeID, int &nElem) const {
			int starti = node2elementStart.get()[0][nodeID];
			int endi = node2elementStart.get()[0][nodeID + 1];
			nElem = endi - starti;
			return node2element.get()[0].data() + starti;
		}

		/**
         * @brief get element IDs associated to an other element ID
         *
         * @param elemID
         * @param nElem Number of elements arround elemID
         * @return int*
         */
		inline int *GetElement2ElementID(int elemID, int &nElem) const {
			int starti = element2elementStart.get()[0][elemID];
			int endi = element2elementStart.get()[0][elemID + 1];
			nElem = endi - starti;
			return element2element.get()[0].data() + starti;
		}

		/**
         * @brief get face IDs associated to an element ID
         *
         * @param elemID
         * @param nElem Number of faces arround elemID
         * @return int*
         */
		inline int *GetElement2FaceID(int elemID, int &nElem) const {
			int starti = element2faceStart.get()[0][elemID];
			int endi = element2faceStart.get()[0][elemID + 1];
			nElem = endi - starti;
			return element2face.get()[0].data() + starti;
		}

		/**
         * @brief get node IDs associated to a face ID
         *
         * @param faceID
         * @param nNode Number of nodes arround faceID
         * @return int*
         */
		inline int *GetFace2NodeID(int faceID, int &nNode) const {
			int starti = face2nodeStart.get()[0][faceID];
			int endi = face2nodeStart.get()[0][faceID + 1];
			nNode = endi - starti;
			return face2node.get()[0].data() + starti;
		}

		/**
         * @brief get element IDs associated to a face ID
         *
         * @param faceID
         * @return int*
         */
		inline int *GetFace2ElementID(int faceID) const {
			int starti = faceID * 2;
			return face2element.get()[0].data() + starti;
		}



	private:
		T _parser;

		std::vector<int> _connectivity;

		// Variables for ghost cells
		std::vector<std::pair<int, std::vector<int>>> MPIGhostCellsIDs;
		std::vector<int> WallGhostCellIDs;
		std::vector<int> WallAdjacentToGhostCellIDs;
		std::vector<int> WallAdjacentFaceIDs;
		std::vector<int> SymmetryGhostCellIDs;
		std::vector<int> SymmetryAdjacentGhostCellIDs;
		std::vector<int> SymmetryAdjacentFaceIDs;
		std::vector<int> FarfieldGhostCellIDs;
		std::vector<int> FarfieldAdjacentToGhostCellIDs;
		std::vector<int> FarfieldAdjacentFaceIDs;
		std::vector<int> facesAroundGhostCells;

		// variable calculees et assignees par connectivity
		std::unique_ptr<std::vector<int>> node2element;
		std::unique_ptr<std::vector<int>> node2elementStart;
		std::unique_ptr<std::vector<int>> element2element;
		std::unique_ptr<std::vector<int>> element2elementStart;
		std::unique_ptr<std::vector<int>> element2face;
		std::unique_ptr<std::vector<int>> element2faceStart;
		std::unique_ptr<std::vector<int>> face2element;
		std::unique_ptr<std::vector<int>> face2nodeStart;
		std::unique_ptr<std::vector<int>> face2node;
		Connectivity connectivityObj;

		// Variable to track connectivity timing
		double startConnectivityTimer;

		/**
         * @brief Finds Ghost cell associated to a MPI (inter partitions) Boundary Conditions.
         * Populates member variables MPIGhostCellsIDs and member variable "_ghostCellID" of GhostCell objects
         * found in vector MPIBoundaryElems of class MeshPartition
		 */
		void updateMPIGhostCells() {

			// use only with solve exectuable (MeshPartition class)
			if constexpr (std::is_same_v<T, E3D::Parser::MeshPartition>) {
				// Find Faces Connected to ghost cells

				int interiorElemsCount = GetMeshInteriorElemCount();
				for (int faceID = 0; faceID < GetnFace(); faceID++) {
					int *p_face2elem = GetFace2ElementID(faceID);
					if (p_face2elem[1] >= interiorElemsCount) {

						facesAroundGhostCells.push_back(faceID);
					}
				}

				// Get the vector of MPI ghost cells

				auto &MPIelems = getMPIelements();

				// Loop through all MPI Ghost cells
				for (auto &[partitionID, elems] : MPIelems) {

					std::pair<int, std::vector<int>> PairghostElemsOfAPartition;
					PairghostElemsOfAPartition.first = partitionID;
					std::vector<int> ghostElemsOfAPartition;
					ghostElemsOfAPartition.reserve(elems.size());

					// Loop though Ghost Cells (MPIelem)
					for (auto &MPIelem : elems) {

						std::vector<int> potentialMPIGhostCell;
						int localElemID = MPIelem.getthisPartitionElementID();
						int SurrElemCount = 0;
						int *p_localElem = GetElement2ElementID(localElemID, SurrElemCount);


						for (int i = 0; i < SurrElemCount; i++) {
							if (p_localElem[i] >= GetMeshInteriorElemCount()) {
								potentialMPIGhostCell.push_back(p_localElem[i]);
							}
						}


						// If local Element connected to only one ghost cell, the searching is done
						if (potentialMPIGhostCell.size() == 1) {
							MPIelem.setGhostCellID(potentialMPIGhostCell[0]);
						}

						// If local Element connected to many ghost cells (farfield, wall..) , MPI ghost cell should be found with face node information
						else if (potentialMPIGhostCell.size() > 1) {
							std::vector<int> localElemIDFaceNodes;
							localElemIDFaceNodes.reserve(MPIelem.getfaceNodeIDs().size());
							for (size_t i = 0; i < MPIelem.getfaceNodeIDs().size(); i++) {
								localElemIDFaceNodes.push_back(MPIelem.getfaceNodeIDs()[i]);
							}
							std::sort(localElemIDFaceNodes.begin(), localElemIDFaceNodes.end());
							bool foundElem = false;
							// Loop through potential Ghost cells (F
							for (auto &potentialGhostCellID : potentialMPIGhostCell) {
								if (foundElem) break;


								int FaceID = 0;

								for (auto &Ghostface : facesAroundGhostCells) {
									int *p_face2elem = GetFace2ElementID(Ghostface);
									if (p_face2elem[1] == potentialGhostCellID) {
										FaceID = Ghostface;
										break;
									}
								}
								int SurrNodeCount = 0;
								int *p_connectedNodes = GetFace2NodeID(FaceID, SurrNodeCount);
								std::vector<int> SurrNodes;

								for (int j = 0; j < SurrNodeCount; j++) {
									SurrNodes.push_back(p_connectedNodes[j]);
								}
								std::sort(SurrNodes.begin(), SurrNodes.end());
								if (SurrNodes == localElemIDFaceNodes) {
									MPIelem.setGhostCellID(potentialGhostCellID);

									ghostElemsOfAPartition.push_back(potentialGhostCellID);
									break;
									foundElem = true;
								}
							}
						}
					}
					PairghostElemsOfAPartition.second = ghostElemsOfAPartition;
					MPIGhostCellsIDs.push_back(PairghostElemsOfAPartition);
				}
			}
		}

		/**
         * @brief Finds Ghost cell associated to a Wall (or airfoil) Boundary Conditions
         * and Interior Cells connected to the Ghost cell.
         * Populates member variables WallGhostCellIDs and WallAdjacentToGhostCellIDs
         */

		void updateWallGhostCell() {
			for (auto &[partitionTag, faces] : GetBoundaryConditionVector()) {
				auto Tag = partitionTag;
				// Transform it to be case insensitive
				std::transform(Tag.begin(), Tag.end(), Tag.begin(), ::tolower);
				if (Tag == "airfoil" || Tag == "wall") {
					WallGhostCellIDs.reserve(faces.size());
					WallAdjacentToGhostCellIDs.reserve(faces.size());
					WallAdjacentFaceIDs.reserve(faces.size());
					for (auto face : faces) {

						auto wallFaceNodes = face.getElemNodes();
						std::sort(wallFaceNodes.begin(), wallFaceNodes.end());
						for (auto &faceConnectedToGC : facesAroundGhostCells) {
							int numNodes = 0;
							int *p_faceToNodes = GetFace2NodeID(faceConnectedToGC, numNodes);
							std::vector<int> tempNodes;
							tempNodes.reserve(numNodes);
							for (int i = 0; i < numNodes; i++) {
								tempNodes.push_back(p_faceToNodes[i]);
							}
							std::sort(tempNodes.begin(), tempNodes.end());
							if (tempNodes == wallFaceNodes) {
								WallGhostCellIDs.push_back(GetFace2ElementID(faceConnectedToGC)[1]);
								WallAdjacentToGhostCellIDs.push_back(GetFace2ElementID(faceConnectedToGC)[0]);
								WallAdjacentFaceIDs.push_back(faceConnectedToGC);
								break;
							}
						}
					}
				}
			}
		}

		/**
         * @brief Finds Ghost cell associated to a Farfield Boundary Conditions
         * and Interior Cells connected to the Ghost cell.
         * Populates member variables FarfieldGhostCellIDs and FarfieldAdjacentToGhostCellIDs
         */
		void updateFarfieldGhostCell() {

			for (auto &[partitionTag, faces] : GetBoundaryConditionVector()) {
				auto Tag = partitionTag;
				// Transform it to be case insensitive
				std::transform(Tag.begin(), Tag.end(), Tag.begin(), ::tolower);
				if (Tag == "farfield") {
					FarfieldGhostCellIDs.reserve(faces.size());
					FarfieldAdjacentToGhostCellIDs.reserve(faces.size());
					FarfieldAdjacentFaceIDs.reserve(faces.size());
					for (auto face : faces) {
						auto wallFaceNodes = face.getElemNodes();
						std::sort(wallFaceNodes.begin(), wallFaceNodes.end());
						for (auto &faceConnectedToGC : facesAroundGhostCells) {
							int numNodes = 0;
							int *p_faceToNodes = GetFace2NodeID(faceConnectedToGC, numNodes);
							std::vector<int> tempNodes;
							tempNodes.reserve(numNodes);
							for (int i = 0; i < numNodes; i++) {
								tempNodes.push_back(p_faceToNodes[i]);
							}
							std::sort(tempNodes.begin(), tempNodes.end());
							if (tempNodes == wallFaceNodes) {
								FarfieldGhostCellIDs.push_back(GetFace2ElementID(faceConnectedToGC)[1]);
								FarfieldAdjacentToGhostCellIDs.push_back(GetFace2ElementID(faceConnectedToGC)[0]);
								FarfieldAdjacentFaceIDs.push_back(faceConnectedToGC);
								break;
							}
						}
					}
				}
			}
		}

		/**
         * @brief Finds Ghost cell associated to a Symmetry Boundary Conditions
         * and Interior Cells connected to the Ghost cell.
         * Populates member variables SymmetryGhostCellIDs and SymmetryAdjacentGhostCellIDs
         */
		void updateSymmetryGhostCell() {
			for (auto &[partitionTag, faces] : GetBoundaryConditionVector()) {
				auto Tag = partitionTag;
				// Transform it to be case insensitive
				std::transform(Tag.begin(), Tag.end(), Tag.begin(), ::tolower);
				if (Tag == "sym") {
					SymmetryGhostCellIDs.reserve(faces.size());
					SymmetryAdjacentGhostCellIDs.reserve(faces.size());
					SymmetryAdjacentFaceIDs.reserve(faces.size());
					for (auto face : faces) {

						auto wallFaceNodes = face.getElemNodes();
						std::sort(wallFaceNodes.begin(), wallFaceNodes.end());
						for (auto &faceConnectedToGC : facesAroundGhostCells) {
							int numNodes = 0;
							int *p_faceToNodes = GetFace2NodeID(faceConnectedToGC, numNodes);
							std::vector<int> tempNodes;
							tempNodes.reserve(numNodes);
							for (int i = 0; i < numNodes; i++) {
								tempNodes.push_back(p_faceToNodes[i]);
							}
							std::sort(tempNodes.begin(), tempNodes.end());
							if (tempNodes == wallFaceNodes) {
								SymmetryGhostCellIDs.push_back(GetFace2ElementID(faceConnectedToGC)[1]);
								SymmetryAdjacentGhostCellIDs.push_back(GetFace2ElementID(faceConnectedToGC)[0]);
								SymmetryAdjacentFaceIDs.push_back(faceConnectedToGC);
								break;
							}
						}
					}
				}
			}
		}
	};

}// namespace E3D
