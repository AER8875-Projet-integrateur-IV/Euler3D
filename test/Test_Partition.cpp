#include <catch2/catch.hpp>
#include "partition/Partition.hpp"
#include <vector>

TEST_CASE("FindContainedElements", "[partition]") {

    std::vector<int> globalSet{1,2,3};
    std::vector<int> index;
    

    SECTION("no match") {
        std::vector<int> subSet{4,5};
        
        E3D::Partition::Partition::FindContainedElements(subSet, globalSet, index);

        REQUIRE(index == std::vector<int>());
    }

    SECTION("Partial match") {
        std::vector<int> subSet{4,2};
        
        E3D::Partition::Partition::FindContainedElements(subSet, globalSet, index);

        REQUIRE(index == std::vector<int>());
    }
    SECTION("full match") {
        std::vector<int> subSet{3,2};
        
        E3D::Partition::Partition::FindContainedElements(subSet, globalSet, index);

        REQUIRE(index == std::vector<int>{2,1});
    }

}