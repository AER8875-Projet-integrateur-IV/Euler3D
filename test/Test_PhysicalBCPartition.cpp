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
#include "utils/Logger.hpp"

/**
 * @details Test the partitionning of physical border conditions for a 2D test
 *      case. The case is graphically represented in docs/test/PhysicalBCPartition.svg
 * 
 */
TEST_CASE("Solve physical border conditions for four 2D squares", "[partition]") {
}

/**
 * @details Test the partitionning of physical border conditions for a 3D test
 *      case. The case is graphically represented in docs/test/cubePartitionning.svg
 * 
 */
TEST_CASE("Solve physical border conditions for 2 3D cubes", "[partition]") {
	std::string fileName = "../../test/mesh/stackedCubes.su2";

	E3D::Mesh<E3D::Parser::SU2MeshParser> mesh(fileName);

	mesh.solveConnectivity();
	int nPart = 2;
	const std::vector<std::string> SU2OuputPath({"tempFile.par", "tempFile.par"});
	E3D::Partition::Partition part(&mesh, nPart);
	auto parts = part.Write(SU2OuputPath, std::vector<int>({0, 1}));

	// part 0
	int partID = 0;
	std::vector<E3D::Parser::Element> redExp{E3D::Parser::Element(9, std::vector<int>{5, 1, 2, 6})};
	std::vector<E3D::Parser::Element> blueExp{E3D::Parser::Element(9, std::vector<int>{6, 2, 3, 7})};
	std::vector<E3D::Parser::Element> greenExp{E3D::Parser::Element(9, std::vector<int>{4, 0, 1, 5})};
	std::vector<E3D::Parser::Element> yellowExp{E3D::Parser::Element(9, std::vector<int>{7, 3, 0, 4})};
	std::vector<E3D::Parser::Element> topExp{E3D::Parser::Element(9, std::vector<int>{6, 7, 4, 5})};
	std::vector<E3D::Parser::Element> botExp{E3D::Parser::Element(9, std::vector<int>{0, 3, 2, 1})};

	std::vector<E3D::Parser::Element> redSol = parts[partID].Markers["red"];
	std::vector<E3D::Parser::Element> blueSol = parts[partID].Markers["blue"];
	std::vector<E3D::Parser::Element> greenSol = parts[partID].Markers["green"];
	std::vector<E3D::Parser::Element> yellowSol = parts[partID].Markers["yellow"];
	std::vector<E3D::Parser::Element> topSol = parts[partID].Markers["top"];
	std::vector<E3D::Parser::Element> botSol;

	int size = parts[partID].Markers.size();
	REQUIRE(redSol == redExp);
	REQUIRE(blueSol == blueExp);
	REQUIRE(greenSol == greenExp);
	REQUIRE(yellowSol == yellowExp);
	REQUIRE(topSol == topExp);
	REQUIRE(size == 5);

	// part 1
	partID = 1;
	redSol = parts[partID].Markers["red"];
	blueSol = parts[partID].Markers["blue"];
	greenSol = parts[partID].Markers["green"];
	yellowSol = parts[partID].Markers["yellow"];
	botSol = parts[partID].Markers["bot"];

	REQUIRE(redSol == redExp);
	REQUIRE(blueSol == blueExp);
	REQUIRE(greenSol == greenExp);
	REQUIRE(yellowSol == yellowExp);
	REQUIRE(botSol == botExp);
	REQUIRE(size == 5);
}

/**
 * @details Test the partitionning of physical border conditions for a 2D test
 *      case. The case is graphically represented in docs/test/tetrahedronPartitionning.svg
 * 
 */
TEST_CASE("Solve physical border conditions for 2 tetrahedron", "[partition]") {
	std::string fileName = "../../test/mesh/stackedTetrahedron.su2";

	E3D::Mesh<E3D::Parser::SU2MeshParser> mesh(fileName);

	mesh.solveConnectivity();
	int nPart = 2;
	const std::vector<std::string> SU2OuputPath({"tempFile.par", "tempFile.par"});
	E3D::Partition::Partition part(&mesh, nPart);
	auto parts = part.Write(SU2OuputPath, std::vector<int>({0, 1}));

	std::vector<E3D::Parser::Element> redExp{E3D::Parser::Element(5, std::vector<int>{0, 1, 3})};
	std::vector<E3D::Parser::Element> blueExp{E3D::Parser::Element(5, std::vector<int>{0, 3, 2})};
	std::vector<E3D::Parser::Element> redSol = parts[0].Markers["red"];
	std::vector<E3D::Parser::Element> blueSol = parts[0].Markers["blue"];
	int size = parts[0].Markers.size();
	REQUIRE(redSol == redExp);
	REQUIRE(blueSol == blueExp);
	REQUIRE(size == 3);
	
	redExp = std::vector<E3D::Parser::Element>{E3D::Parser::Element(5, std::vector<int>{0, 3, 2})};
	blueExp = std::vector<E3D::Parser::Element>{E3D::Parser::Element(5, std::vector<int>{2, 3, 1})};
	redSol = parts[1].Markers["red"];
	blueSol = parts[1].Markers["blue"];
	size = parts[1].Markers.size();
	REQUIRE(redSol == redExp);
	REQUIRE(blueSol == blueExp);
	REQUIRE(size == 3);
}
