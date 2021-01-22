#include <catch2/catch.hpp>
#include "partition/SU2Writer.hpp"
#include "parser/SU2MeshParser.hpp"
#include <iostream>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

TEST_CASE("SU2Writer class test", "[partition]") {

    std::string targetPath("tmp_Test_SU2Writer.su2");
    SU2Writer writer(targetPath, 3);    
    std::ifstream file(targetPath);

    std::string line;
    SECTION("Coord writing with 2 3D nodes") {
        E3D::Parser::Node node1(std::vector<double>{1.1, 2e4, 3e14});
        E3D::Parser::Node node2(std::vector<double>{0, 2e-4, 1});

        std::vector<E3D::Parser::Node> nodes{node1, node2};

        writer.WriteCoord(3,nodes);

        std::getline(file, line);
        REQUIRE("NPOIN= 2" == line);
        std::getline(file, line);
        REQUIRE("" == line);
        std::getline(file, line);
        REQUIRE("1.100e+00 2.000e+04 3.000e+14" == line);
        std::getline(file, line);
        REQUIRE("0.000e+00 2.000e-04 1.000e+00" == line);
        std::getline(file, line);
        REQUIRE("" == line);
        
        REQUIRE(file.peek() == std::ifstream::traits_type::eof());
    }

    SECTION("Element 2 Node Connectivity with 2 elements"){
        E3D::Parser::Element elem1(3, std::vector<int>{1,3000});
        E3D::Parser::Element elem2(13, std::vector<int>{1,2,3,4,5,6,7,8});
        std::vector<E3D::Parser::Element> elemVector{elem1, elem2};

        writer.WriteElement2Node(elemVector);

        std::getline(file, line);
        REQUIRE("NELEM= 2" == line);
        std::getline(file, line);
        REQUIRE("" == line);
        std::getline(file, line);
        REQUIRE("3 1 3000" == line);
        std::getline(file, line);
        REQUIRE("13 1 2 3 4 5 6 7 8" == line);
        std::getline(file, line);
        REQUIRE("" == line);
        
        REQUIRE(file.peek() == std::ifstream::traits_type::eof());        
    }

    SECTION("Marker connectivity for two markers with 2 elements each"){
        E3D::Parser::Element elem1(3, std::vector<int>{1,2});
        E3D::Parser::Element elem2(3, std::vector<int>{2,3});   
        std::vector<E3D::Parser::Element> elemVector1{elem1, elem2};
        std::pair<std::string,std::vector<E3D::Parser::Element>> BC1("farfield", elemVector1);

        E3D::Parser::Element elem3(3, std::vector<int>{3,4});
        E3D::Parser::Element elem4(3, std::vector<int>{4,5});   
        std::vector<E3D::Parser::Element> elemVector2{elem3, elem4};
        std::pair<std::string,std::vector<E3D::Parser::Element>> BC2("wall", elemVector2);

        E3D::Parser::BC_Structure Markers{BC1, BC2};

        writer.WriteMarker(Markers);

        std::getline(file, line);
        REQUIRE("NMARK= 2" == line);
        std::getline(file, line);
        REQUIRE("MARKER_TAG= farfield" == line);
        std::getline(file, line);
        REQUIRE("MARKER_ELEMS= 2" == line);
        std::getline(file, line);
        REQUIRE("3 1 2" == line);
        std::getline(file, line);
        REQUIRE("3 2 3" == line);
        std::getline(file, line);
        REQUIRE("MARKER_TAG= wall" == line);
        std::getline(file, line);
        REQUIRE("MARKER_ELEMS= 2" == line);
        std::getline(file, line);
        REQUIRE("3 3 4" == line);
        std::getline(file, line);
        REQUIRE("3 4 5" == line);
        std::getline(file, line);
        REQUIRE("" == line);

        REQUIRE(file.peek() == std::ifstream::traits_type::eof());             
    }

    SECTION("Inter Partition connectivity"){

    }
    file.close();
    remove(targetPath.c_str());

}