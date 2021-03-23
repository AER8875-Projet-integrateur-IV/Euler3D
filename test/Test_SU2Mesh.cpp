#include "partition/Partition.hpp"
#include <catch2/catch.hpp>
#include <string>

TEST_CASE("SU2Mesh add marker", "[partition]") {
	E3D::Partition::SU2Mesh mesh;
	std::vector<int> nodeVec{1, 2};
	int VTKId = 3;
	std::string tag1 = "testTag1";
	std::string tag2 = "testTag2";

	SECTION("Add a new marker to an empty Bordercondition object") {
		mesh.AddMarkerElement(tag1, VTKId, nodeVec.data(), nodeVec.size());
		REQUIRE(mesh.Markers.size() == 1);
	}
	SECTION("Add a new marker to a non empty Bordercondition object") {
		mesh.AddMarkerElement(tag1, VTKId, nodeVec.data(), nodeVec.size());
		mesh.AddMarkerElement(tag2, VTKId, nodeVec.data(), nodeVec.size());
		REQUIRE(mesh.Markers.size() == 2);
	}
	SECTION("Add a new element to an existing marker") {
		mesh.AddMarkerElement(tag1, VTKId, nodeVec.data(), nodeVec.size());
		mesh.AddMarkerElement(tag2, VTKId, nodeVec.data(), nodeVec.size());
		mesh.AddMarkerElement(tag2, VTKId, nodeVec.data(), nodeVec.size());

		REQUIRE(mesh.Markers.size() == 2);

		REQUIRE(mesh.Markers["testTag1"].size() == 1);
		REQUIRE(mesh.Markers["testTag2"].size() == 2);
	}
}

TEST_CASE("SU2Mesh LocalNode2Global", "[partition]") {
	E3D::Partition::SU2Mesh mesh;
	mesh.ID = 1;

	// build localNode2Global
	std::vector<int> localNode2Global{1, 2, 4, 5,
	                                  2, 3, 5, 6,
	                                  4, 5, 6, 7, 8, 9};
	std::vector<int> localNode2GlobalStart{0, 4, 8, 14};

	mesh.SetLocal2GlobalConnectivy(localNode2Global, localNode2GlobalStart);

	SECTION("first index") {
		int globID = mesh.LocalNode2global(0);
		REQUIRE(globID == 2);
	}

	SECTION("last index") {
		int globID = mesh.LocalNode2global(3);
		REQUIRE(globID == 6);
	}
}