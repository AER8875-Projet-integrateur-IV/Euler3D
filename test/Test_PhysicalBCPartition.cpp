/**
 * @file Test_PhysicalBCPartition.cpp
 * @author Abraham Bhérer-Constant (abraham.bconstant@gmail.com)
 * @brief Test file for Partition class
 * @date 2021-02-05
 * 
 * 
 */
#include "partition/PhysicalBCPartition.hpp"
#include <catch2/catch.hpp>
#include <vector>

TEST_CASE("FindContainedElements", "[partition]") {

	std::vector<int> globalSet{1, 2, 3};
	std::vector<int> index;


	SECTION("no match") {
		std::vector<int> subSet{4, 5};

		E3D::Partition::PhysicalBCPartition::FindContainedElements(subSet, globalSet, index);

		REQUIRE(index == std::vector<int>());
	}

	SECTION("Partial match") {
		std::vector<int> subSet{4, 2};

		E3D::Partition::PhysicalBCPartition::FindContainedElements(subSet, globalSet, index);

		REQUIRE(index == std::vector<int>());
	}
	SECTION("full match") {
		std::vector<int> subSet{3, 2};

		E3D::Partition::PhysicalBCPartition::FindContainedElements(subSet, globalSet, index);

		REQUIRE(index == std::vector<int>{2, 1});
	}
}

/**
 * @details Test the partitionning of physical border conditions for a 2D test
 *      case. The case is graphically represented in docs/test/PhysicalBCPartition.svg
 * 
 */
TEST_CASE("Solve physical border conditions for four 2D squares", "[partition],[PhysicalBC]") {
	// build localNode2Global
	std::vector<int> localNode2Global{0, 1, 3, 4,       // red
	                                  1, 2, 4, 5,       // blue
	                                  3, 4, 5, 6, 7, 8};// green
	std::vector<int> localNode2GlobalStart{0, 4, 8, 14};

	// building SU2Mesh objects
	E3D::Partition::SU2Mesh green;
	green.elem2nodeStart = std::vector<int>{0, 4, 8};
	green.elem2node = std::vector<int>{0, 1, 4, 3, 1, 2, 5, 4};
	green.NDIM = 2;
	green.NELEM = 2;
	green.NPOIN = 6;
	green.ID = 2;
	green.SetLocal2GlobalConnectivy(localNode2Global, localNode2GlobalStart);
	green.physicalBorderElements = std::vector<int>{0, 1};

	E3D::Partition::SU2Mesh red;
	red.elem2nodeStart = std::vector<int>{0, 4};
	red.elem2node = std::vector<int>{0, 1, 3, 2};
	red.NDIM = 2;
	red.NELEM = 1;
	red.NPOIN = 4;
	red.ID = 0;
	red.SetLocal2GlobalConnectivy(localNode2Global, localNode2GlobalStart);
	red.physicalBorderElements = std::vector<int>{0};

	E3D::Partition::SU2Mesh blue;
	blue.elem2nodeStart = std::vector<int>{0, 4};
	blue.elem2node = std::vector<int>{0, 1, 3, 2};
	blue.NDIM = 2;
	blue.NELEM = 1;
	blue.NPOIN = 4;
	blue.ID = 1;
	blue.SetLocal2GlobalConnectivy(localNode2Global, localNode2GlobalStart);
	blue.physicalBorderElements = std::vector<int>{0};

	std::vector<E3D::Partition::SU2Mesh> parts{red, blue, green};

	// Build border conditions
	std::pair<std::string, std::vector<E3D::Parser::Element>> south;
	south.first = "south";
	south.second = std::vector<E3D::Parser::Element>{
	        E3D::Parser::Element(3, std::vector<int>{8, 7}),
	        E3D::Parser::Element(3, std::vector<int>{7, 6}),
	        E3D::Parser::Element(3, std::vector<int>{6, 3}),
	        E3D::Parser::Element(3, std::vector<int>{3, 0})};

	std::pair<std::string, std::vector<E3D::Parser::Element>> north;
	north.first = "north";
	north.second = std::vector<E3D::Parser::Element>{
	        E3D::Parser::Element(3, std::vector<int>{8, 5}),
	        E3D::Parser::Element(3, std::vector<int>{5, 2}),
	        E3D::Parser::Element(3, std::vector<int>{2, 1}),
	        E3D::Parser::Element(3, std::vector<int>{1, 0})};

	E3D::Parser::BC_Structure BC{south, north};

	// Run partitionning
	E3D::Partition::PhysicalBCPartition::Solve(BC, parts);

	SECTION("Red partition") {
		std::vector<E3D::Parser::Element> northExp{E3D::Parser::Element(3, std::vector<int>{1, 0})};
		std::vector<E3D::Parser::Element> southExp{E3D::Parser::Element(3, std::vector<int>{2, 0})};
		std::vector<E3D::Parser::Element> northSol = parts[0].Markers["north"];
		std::vector<E3D::Parser::Element> southSol = parts[0].Markers["south"];
		int size = parts[0].Markers.size();
		REQUIRE(northSol == northExp);
		REQUIRE(southSol == southExp);
		REQUIRE(size == 2);
	}

	SECTION("Blue partition") {
		std::vector<E3D::Parser::Element> northExp{E3D::Parser::Element(3, std::vector<int>{3, 1}),
		                                           E3D::Parser::Element(3, std::vector<int>{1, 0})};
		std::vector<E3D::Parser::Element> northSol = parts[1].Markers["north"];
		REQUIRE(northSol == northExp);
		REQUIRE(parts[1].Markers.size() == 1);
	}

	SECTION("Green partition") {
		std::vector<E3D::Parser::Element> northExp{E3D::Parser::Element(3, std::vector<int>{5, 2})};
		std::vector<E3D::Parser::Element> southExp{E3D::Parser::Element(3, std::vector<int>{5, 4}),
		                                           E3D::Parser::Element(3, std::vector<int>{4, 3}),
		                                           E3D::Parser::Element(3, std::vector<int>{3, 0})};
		std::vector<E3D::Parser::Element> northSol = parts[2].Markers["north"];
		std::vector<E3D::Parser::Element> southSol = parts[2].Markers["south"];
		REQUIRE(northSol == northExp);
		REQUIRE(southSol == southExp);
		REQUIRE(parts[2].Markers.size() == 2);
	}
}


/**
 * @details Test the partitionning of physical border conditions for a 2D test
 *      case. The case is graphically represented in docs/test/tetrahedronPartitionning.svg
 * 
 */
TEST_CASE("Solve physical border conditions for 2 tetrahedron", "[partition],[PhysicalBC]") {
	// build localNode2Global
	std::vector<int> localNode2Global{0, 1, 2, 3, // top
	                                  1, 2, 3, 4};// bot
	std::vector<int> localNode2GlobalStart{0, 4, 8};

	// building SU2Mesh objects
	E3D::Partition::SU2Mesh top;
	top.elem2nodeStart = std::vector<int>{0, 4};
	top.elem2node = std::vector<int>{0, 1, 2, 3};
	top.NDIM = 3;
	top.NELEM = 1;
	top.NPOIN = 4;
	top.ID = 0;
	top.SetLocal2GlobalConnectivy(localNode2Global, localNode2GlobalStart);
	top.physicalBorderElements = std::vector<int>{0};

	E3D::Partition::SU2Mesh bot;
	bot.elem2nodeStart = std::vector<int>{0, 4};
	bot.elem2node = std::vector<int>{0, 1, 2, 3};
	bot.NDIM = 3;
	bot.NELEM = 1;
	bot.NPOIN = 4;
	bot.ID = 1;
	bot.SetLocal2GlobalConnectivy(localNode2Global, localNode2GlobalStart);
	bot.physicalBorderElements = std::vector<int>{0};

	std::vector<E3D::Partition::SU2Mesh> parts{top, bot};

	// Build border conditions
	std::pair<std::string, std::vector<E3D::Parser::Element>> red;
	red.first = "red";
	red.second = std::vector<E3D::Parser::Element>{
	        E3D::Parser::Element(5, std::vector<int>{0, 1, 3}),
	        E3D::Parser::Element(5, std::vector<int>{1, 4, 3})};

	std::pair<std::string, std::vector<E3D::Parser::Element>> blue;
	blue.first = "blue";
	blue.second = std::vector<E3D::Parser::Element>{
	        E3D::Parser::Element(5, std::vector<int>{0, 3, 2}),
	        E3D::Parser::Element(5, std::vector<int>{3, 4, 2})};

	E3D::Parser::BC_Structure BC{red, blue};

	// Run partitionning
	E3D::Partition::PhysicalBCPartition::Solve(BC, parts);

	SECTION("top partition") {
		std::vector<E3D::Parser::Element> redExp{E3D::Parser::Element(5, std::vector<int>{0, 1, 3})};
		std::vector<E3D::Parser::Element> blueExp{E3D::Parser::Element(5, std::vector<int>{0, 3, 2})};
		std::vector<E3D::Parser::Element> redSol = parts[0].Markers["red"];
		std::vector<E3D::Parser::Element> blueSol = parts[0].Markers["blue"];
		int size = parts[0].Markers.size();
		REQUIRE(redSol == redExp);
		REQUIRE(blueSol == blueExp);
		REQUIRE(size == 2);
	}

	SECTION("bot partition") {
		std::vector<E3D::Parser::Element> redExp{E3D::Parser::Element(5, std::vector<int>{0, 3, 2})};
		std::vector<E3D::Parser::Element> blueExp{E3D::Parser::Element(5, std::vector<int>{2, 3, 1})};
		std::vector<E3D::Parser::Element> redSol = parts[1].Markers["red"];
		std::vector<E3D::Parser::Element> blueSol = parts[1].Markers["blue"];
		int size = parts[1].Markers.size();
		REQUIRE(redSol == redExp);
		REQUIRE(blueSol == blueExp);
		REQUIRE(size == 2);
	}
}
