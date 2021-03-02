/**
 * @file Test_Partition.cpp
 * @author Abraham Bhérer-Constant (abraham.bconstant@gmail.com)
 * @brief Test file for Partition class
 * @date 2021-02-05
 * 
 * 
 */
#include "connectivity/Connectivity.hpp"
#include "mesh/Mesh.hpp"
#include "partition/Partition.hpp"
#include <catch2/catch.hpp>
#include <string>
#include <vector>

/**
 * @details Test the partitionning of physical border conditions for a 2D test
 *      case. The case is graphically represented in docs/test/PhysicalBCPartition.svg
 * 
 */
TEST_CASE("Solve physical border conditions for four 2D squares", "[partition]") {
}


/**
 * @details Test the partitionning of physical border conditions for a 2D test
 *      case. The case is graphically represented in docs/test/tetrahedronPartitionning.svg
 * 
 */
TEST_CASE("Solve physical border conditions for 2 tetrahedron", "[partition]") {
	E3D::Mesh<E3D::Parser::SU2MeshParser> mesh("../../test/mesh/stackedTetrahedron.su2");
	std::vector<E3D::Partition::SU2Mesh> parts;
	mesh.solveConnectivity();
	int nPart = 2;
	E3D::Partition::Partition part(&mesh, nPart);
	std::vector<std::string> path({"tempFile.par", "tempFile.par"});
	parts = part.Write(path, std::vector<int>({0, 1}));

	SECTION("top partition") {
		std::vector<E3D::Parser::Element> redExp{E3D::Parser::Element(5, std::vector<int>{0, 1, 3})};
		std::vector<E3D::Parser::Element> blueExp{E3D::Parser::Element(5, std::vector<int>{0, 3, 2})};
		std::vector<E3D::Parser::Element> redSol = parts[0].Markers["red"];
		std::vector<E3D::Parser::Element> blueSol = parts[0].Markers["blue"];
		int size = parts[0].Markers.size();
		REQUIRE(redSol == redExp);
		REQUIRE(blueSol == blueExp);
		REQUIRE(size == 3);
	}

	SECTION("bot partition") {
		std::vector<E3D::Parser::Element> redExp{E3D::Parser::Element(5, std::vector<int>{0, 3, 2})};
		std::vector<E3D::Parser::Element> blueExp{E3D::Parser::Element(5, std::vector<int>{2, 3, 1})};
		std::vector<E3D::Parser::Element> redSol = parts[1].Markers["red"];
		std::vector<E3D::Parser::Element> blueSol = parts[1].Markers["blue"];
		int size = parts[1].Markers.size();
		REQUIRE(redSol == redExp);
		REQUIRE(blueSol == blueExp);
		REQUIRE(size == 3);
	}
}
