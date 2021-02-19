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