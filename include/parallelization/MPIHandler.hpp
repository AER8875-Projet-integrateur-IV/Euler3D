#pragma once

#include "parser/GhostCell.hpp"
#include "solver/FlowField.hpp"
#include <mpi.h>
#include <vector>

namespace E3D::Parallel {

	/**
     * @brief : Class to handle MPI communications between partitions
     */
	class MPIHandler {
	public:
		/**
         *  @brief : ctor initializes MPI and populate rank,size
         *  parameters argc and argv are required by MPI_init function but they are
         *  used in this class
         */
		MPIHandler(int argc, char *argv[]);


		/**
         * @brief Update ID of neighbor partitions and their count in private member variables
         * @param adjacentPartitionsID : vector of ints holding neighbor partitions IDs
         */
		inline void updateAdjacentPartitionsInfo(const std::vector<int> adjacentPartitionsID) {
			_adjacentPartitionsID = adjacentPartitionsID;
			_nAdjacentPartition = adjacentPartitionsID.size();
		}

		/**
         *  @brief set the requester ID member variable
         */
		inline void updateRequesterID(const std::vector<std::pair<int, std::vector<E3D::Parser::GhostCell>>> reqID) {
			_requesterID = reqID;
			_senderID = reqID;
		};

		/**
         * @brief sort requester ID in destination partition increasing order -> this member function populate
         * _senderID private member variable from _requesterID
         */
		void sortInterfaceRequester();

		/**
         * @brief : send flowfield variables to neighbor partitions via point to point communication
         * TODO add appropriate arguments to the function
         */
		void updateFlowField(Solver::FlowField &) const;


		/**
          * @brief Compute and broadcast maximum Residual through collective communication
          * TODO add appropriate arguments to the function
          */
		void updateRMS();


		inline int getRankID() const { return _rankID; }

		inline int getPoolSize() const { return _poolSize; }

		inline const std::vector<std::pair<int, std::vector<E3D::Parser::GhostCell>>> &getRequesterID() const { return _requesterID; }


        inline const std::vector<std::pair<int, std::vector<E3D::Parser::GhostCell>>> &getSenderID() const { return _senderID; }
		/**
         * @brief Finalize this MPI process
         */
		inline void finalize() { MPI_Finalize(); }

	private:
		std::vector<std::pair<int, std::vector<E3D::Parser::GhostCell>>> _requesterID; /** @brief vector of vectors holding IDs of MPI boundary elements sorted
                                                              * in increasing order,  in following format :
                                                              * [  [MpiElements]  --> For adjacentPartitionsID[0]
                                                              *    [MpiElements]  --> For adjacentPartitionsID[1]
                                                              *    [MpiElements]  --> For adjacentPartitionsID[2] ]*/

		std::vector<std::pair<int, std::vector<E3D::Parser::GhostCell>>> _senderID; /** vector of vectors holding IDs of MPI boundary elements sorted in
                                                             * destination partition increasing order. Same format as requesterID */

		std::vector<int> _adjacentPartitionsID; /** @brief vector holding IDs of physically connected partitions */

		MPI_Request _request;
		MPI_Status _status;

		int _rankID = -1;            /** @brief rank id of this process */
		int _poolSize = 0;           /** @brief Number of processes in the communicator */
		int _nAdjacentPartition = 0; /** @brief number of partitions physically connected to this partitions */

		static constexpr int RHO_TAG = 0;
		static constexpr int U_TAG = 1;
		static constexpr int V_TAG = 2;
		static constexpr int W_TAG = 3;
		static constexpr int E_TAG = 4;


		void updateRho(std::vector<double> &) const;

		void updateU(std::vector<double> &) const;

		void updateV(std::vector<double> &) const;

		void updateW(std::vector<double> &) const;

		void updateE(std::vector<double> &) const;
	};
}// namespace E3D::Parallel
