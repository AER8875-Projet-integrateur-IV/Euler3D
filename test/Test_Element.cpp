#include <catch2/catch.hpp>
#include <vector>
#include "parser/Element.hpp"

TEST_CASE("Element == operator", "[mesh]") {
    E3D::Parser::Element elem1(3,std::vector<int>{1,2});
    E3D::Parser::Element elem2(3,std::vector<int>{3,2});
    
    SECTION("equal") {
        REQUIRE(elem1 == elem1);
    }
    SECTION("not equal"){
        REQUIRE_FALSE(elem1 == elem2);
    }
}