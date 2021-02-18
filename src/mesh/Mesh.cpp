#include "mesh/Mesh.hpp"
using namespace E3D;

Mesh::Mesh(const std::string& fileName) : _parser(fileName) {
    _parser.printInfo();
}
