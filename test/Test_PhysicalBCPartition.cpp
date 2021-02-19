/**
 * @file Test_Partition.cpp
 * @author Abraham Bhérer-Constant (abraham.bconstant@gmail.com)
 * @brief Test file for Partition class
 * @date 2021-02-05
 * 
 * 
 */
#include <catch2/catch.hpp>
#include "partition/PhysicalBCPartition.hpp"
#include <vector>

TEST_CASE("FindContainedElements", "[partition]") {

    std::vector<int> globalSet{1,2,3};
    std::vector<int> index;
    

    SECTION("no match") {
        std::vector<int> subSet{4,5};
        
        E3D::Partition::PhysicalBCPartition::FindContainedElements(subSet, globalSet, index);

        REQUIRE(index == std::vector<int>());
    }

    SECTION("Partial match") {
        std::vector<int> subSet{4,2};
        
        E3D::Partition::PhysicalBCPartition::FindContainedElements(subSet, globalSet, index);

        REQUIRE(index == std::vector<int>());
    }
    SECTION("full match") {
        std::vector<int> subSet{3,2};
        
        E3D::Partition::PhysicalBCPartition::FindContainedElements(subSet, globalSet, index);

        REQUIRE(index == std::vector<int>{2,1});
    }

}

TEST_CASE("Solve physical border conditions in 2D", "[partition]"){
    // build localNode2Global
    std::vector<int> localNode2Global{1, 2, 4, 5,       // red 
                                      2, 3, 5, 6,       // blue
                                      4, 5, 6, 7, 8, 9};// green
    std::vector<int> localNode2GlobalStart{0,4,8,14};
    
    // building SU2Mesh objects
    E3D::Partition::SU2Mesh green;
    green.elem2nodeStart = std::vector<int>{0, 4, 8};
    green.elem2node = std::vector<int>{1, 2, 5, 4, 2, 3, 6, 5};
    green.NDIM = 2;
    green.NELEM = 2;
    green.NPOIN = 6;
    green.ID = 2;
    green.SetLocal2GlobalConnectivy(localNode2Global, localNode2GlobalStart);

    E3D::Partition::SU2Mesh red;
    red.elem2nodeStart = std::vector<int>{0, 4};
    red.elem2node = std::vector<int>{1, 2, 4, 3};
    red.NDIM = 2;
    red.NELEM = 1;
    red.NPOIN = 4;
    red.ID = 0;
    red.SetLocal2GlobalConnectivy(localNode2Global, localNode2GlobalStart);

    E3D::Partition::SU2Mesh blue;
    blue.elem2nodeStart = std::vector<int>{0, 4};
    blue.elem2node = std::vector<int>{1, 2, 4, 3};
    blue.NDIM = 2;
    blue.NELEM = 1;
    blue.NPOIN = 4;
    blue.ID = 1;
    blue.SetLocal2GlobalConnectivy(localNode2Global, localNode2GlobalStart);

    std::vector<E3D::Partition::SU2Mesh> parts{red, blue, green};

    // Build border conditions
    std::pair<std::string,std::vector<E3D::Parser::Element>> south;
    south.first = "south";
    south.second = std::vector<E3D::Parser::Element>{
            E3D::Parser::Element(3, std::vector<int>{8,9}),
            E3D::Parser::Element(3, std::vector<int>{7,8}),
            E3D::Parser::Element(3, std::vector<int>{4,7}),
            E3D::Parser::Element(3, std::vector<int>{1,4})};

    std::pair<std::string,std::vector<E3D::Parser::Element>> north;
    south.first = "north";
    south.second = std::vector<E3D::Parser::Element>{
            E3D::Parser::Element(3, std::vector<int>{9,6}),
            E3D::Parser::Element(3, std::vector<int>{6,3}),
            E3D::Parser::Element(3, std::vector<int>{3,2}),
            E3D::Parser::Element(3, std::vector<int>{2,1})};    

    E3D::Parser::BC_Structure BC{south, north};

    // Run partitionning
    E3D::Partition::PhysicalBCPartition::Solve(BC, parts);

}