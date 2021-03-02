#include "parser/Element.hpp"
#include <catch2/catch.hpp>
#include <vector>

TEST_CASE("Element == operator", "[mesh]") {
	E3D::Parser::Element elem1(3, std::vector<int>{1, 2});
	E3D::Parser::Element elem2(3, std::vector<int>{3, 2});

	E3D::Parser::Element elem3(3, std::vector<int>{1, 2, 3, 4});
	E3D::Parser::Element elem4(3, std::vector<int>{3, 4, 1, 2});

	SECTION("equal") {
		REQUIRE(elem1 == elem1);
	}
	SECTION("not equal") {
		REQUIRE_FALSE(elem1 == elem2);
	}
	SECTION("equal but different order") {
		REQUIRE(elem3 == elem4);
	}
}