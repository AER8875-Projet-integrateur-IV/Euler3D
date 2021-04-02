#pragma once

#include "mesh/Mesh.hpp"
#include "parallelization/MPIHandler.hpp"
#include "parser/SimConfig.hpp"
#include "solver/ConservativeVar.hpp"
#include "solver/FlowField.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace E3D::Solver {

    class WriteSolution {

    public:
        WriteSolution(E3D::Solver::FlowField &flowField,
                      const E3D::Mesh<E3D::Parser::MeshPartition> &localMesh,
                      const E3D::Parser::SimConfig &config,
                      const E3D::Parallel::MPIHandler &e3d_mpi);
        ~WriteSolution();

    private:
        E3D::Solver::FlowField &_flowField;
        const E3D::Mesh<E3D::Parser::MeshPartition> &_localMesh;
        int _rank;
        std::string _fileName;
        std::ofstream _Solfile;
        std::ofstream _Boundaryfile;

        void write();
        void writeSolutionFile();
        void writeBoundaryFile();
    };
}// namespace E3D::Solver