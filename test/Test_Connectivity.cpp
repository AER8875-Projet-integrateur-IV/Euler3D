#include <catch2/catch.hpp>
#include "parser/SU2MeshParser.hpp"
#include <iostream>
#include "connectivity/Connectivity.hpp"
#include "mesh/Mesh.hpp"

TEST_CASE("Connectivity class test", "[connectivity]") {

    // Initialize SU2MeshParser class for testing
    std::string filename = "../../test/mesh/StructuredBlock_8.su2";

    E3D::Mesh<E3D::Parser::SU2MeshParser> mesh(filename);


    mesh.solveConnectivity();

    SECTION("Number of faces") {
        // Test number of dimensions
        const int nFace = mesh.GetnFace();
        REQUIRE(nFace == 36);
    }

    SECTION("node2element linked list") {

        int size;
        int* ptr;

		ptr = mesh.GetNode2ElementID(0, size);
        std::vector<int> node2element0(ptr, ptr+size);
        std::vector<int> correctnode2element0 = {0};
        REQUIRE(node2element0 == correctnode2element0);

        ptr = mesh.GetNode2ElementID(13,size);
        std::vector<int> node2element13(ptr, ptr+size);
        std::vector<int> correctnode2element13 = {0,1,2,3,4,5,6,7};
        REQUIRE(node2element13 == correctnode2element13);


        ptr = mesh.GetNode2ElementID(20,size);
        std::vector<int> node2element20(ptr, ptr+size);
      
        std::vector<int> correctnode2element20 = {5};
        REQUIRE(node2element20 == correctnode2element20);

    }

	SECTION("element2element linked list") {
		int size;
		int *ptr;

		ptr = mesh.GetElement2ElementID(0, size);
		std::vector<int> element2element0(ptr, ptr + size);
		std::vector<int> correctelement2element0 = {8, 9, 1, 2, 10, 4};
		REQUIRE(element2element0 == correctelement2element0);

		ptr = mesh.GetElement2ElementID(3, size);
		std::vector<int> element2element3(ptr, ptr + size);
		std::vector<int> correctelement2element3 = {17, 1, 18, 19, 2, 7};
		REQUIRE(element2element3 == correctelement2element3);


		ptr = mesh.GetElement2ElementID(7, size);
		std::vector<int> element2element7(ptr, ptr + size);
		std::vector<int> correctelement2element7 = {3, 5, 29, 30, 6, 31};
		REQUIRE(element2element7 == correctelement2element7);
	}

	SECTION("element2face linked list") {
		int size;
		int *ptr;

		ptr = mesh.GetElement2FaceID(0, size);
		std::vector<int> element2face0(ptr, ptr + size);
		std::vector<int> correctelement2face0 = {12, 13, 0, 1, 14, 2};
		REQUIRE(element2face0 == correctelement2face0);

		ptr = mesh.GetElement2FaceID(3, size);
		std::vector<int> element2face3(ptr, ptr + size);
		std::vector<int> correctelement2face3 = {21, 3, 22, 23, 5, 7};
		REQUIRE(element2face3 == correctelement2face3);


		ptr = mesh.GetElement2FaceID(7, size);
		std::vector<int> element2face7(ptr, ptr + size);
		std::vector<int> correctelement2face7 = {7, 10, 33, 34, 11, 35};
		REQUIRE(element2face7 == correctelement2face7);
	}

	SECTION("face2element linked list") {
		int size = 2;
		int *ptr;

		ptr = mesh.GetFace2ElementID(0);
		std::vector<int> face2element0(ptr, ptr + size);
		std::vector<int> correctface2element0 = {0, 1};
		REQUIRE(face2element0 == correctface2element0);

		ptr = mesh.GetFace2ElementID(29);
		std::vector<int> face2element29(ptr, ptr + size);
		std::vector<int> correctface2element29 = {5, 25};
		REQUIRE(face2element29 == correctface2element29);


		ptr = mesh.GetFace2ElementID(35);
		std::vector<int> face2element35(ptr, ptr + size);
		std::vector<int> correctface2element35 = {7, 31};
		REQUIRE(face2element35 == correctface2element35);
	}

	SECTION("face2node linked list") {
		int size;
		int *ptr;

		ptr = mesh.GetFace2NodeID(0, size);
		std::vector<int> face2node0(ptr, ptr + size);
		std::vector<int> correctface2node0 = {1, 4, 13, 10};
		REQUIRE(face2node0 == correctface2node0);

		ptr = mesh.GetFace2NodeID(29, size);
		std::vector<int> face2node29(ptr, ptr + size);
		std::vector<int> correctface2node29 = {19, 20, 23, 22};
		REQUIRE(face2node29 == correctface2node29);


		ptr = mesh.GetFace2NodeID(35, size);
		std::vector<int> face2node35(ptr, ptr + size);
		std::vector<int> correctface2node35 = {22, 23, 26, 25};
		REQUIRE(face2node35 == correctface2node35);
	}
}
