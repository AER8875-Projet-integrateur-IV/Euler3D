#include <catch2/catch.hpp>
#include "parser/SU2MeshParser.hpp"
#include <iostream>
#include "connectivity/Connectivity.hpp"
#include "mesh/Mesh.hpp"

TEST_CASE("Connectivity class test", "[connectivity]") {

    // Initialize SU2MeshParser class for testing
    std::string filename = "../../test/mesh/StructuredBlock_8.su2";
    E3D::Mesh mesh(filename);

    mesh.solveConnectivity();

    SECTION("Number of faces") {
        // Test number of dimensions
        const int nFace = mesh.GetnFace();
        REQUIRE(nFace == 36);
    }

    SECTION("node2element linked list") {

        const std::vector<int> node2element0 = mesh.Getnode2element(0);
        std::vector<int> correctnode2element0 = {0};
        REQUIRE(node2element0 == correctnode2element0);

        const std::vector<int> node2element13 = mesh.Getnode2element(13);
        std::vector<int> correctnode2element13 = {0,1,2,3,4,5,6,7};
        REQUIRE(node2element13 == correctnode2element13);

        const std::vector<int> node2element20 = mesh.Getnode2element(20);
        std::vector<int> correctnode2element20 = {5};
        REQUIRE(node2element20 == correctnode2element20);

    }

}
