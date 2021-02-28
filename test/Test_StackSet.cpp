/**
 * @file Test_StackSet.cpp
 * @author Abraham Bhérer-Constant (abraham.bconstant@gmail.com)
 * @date 2021-02-26
 * 
 * 
 */
#include <catch2/catch.hpp>
#include "partition/StackSet.hpp"
#include <vector>
TEST_CASE("StackList tests", "[partition]") {
    std::vector<int> values{0,1,2};
    StackSet<int> ss(values.begin(), values.end());

    SECTION("[] operator"){
        REQUIRE(ss[0] == 0);
        REQUIRE(ss[1] == 1);
        REQUIRE(ss[2] == 2);
    }

    SECTION("toFront test"){
        int param = 2;
        ss.toFront(param);
        REQUIRE(ss[0] == 2);
        REQUIRE(ss[1] == 0);
        REQUIRE(ss[2] == 1);        
    }

    SECTION("range based for loop"){
        int expected = 0;
        for(auto val : ss){
            REQUIRE(val == expected++);
        }
    }
}