#include <catch2/catch.hpp>
#include "partition/Partition.hpp"
#include <string>

TEST_CASE("SU2Mesh class test", "[partition]") {
    E3D::Partition::SU2Mesh mesh;
    std::vector<int> nodeVec {1,2};
    int VTKId =3;
    std::string tag1 = "testTag1";
    std::string tag2 = "testTag2";

    SECTION("Add a new marker to an empty Bordercondition object") {
        mesh.AddMarkerElement(tag1, VTKId, nodeVec.data(), nodeVec.size());
        REQUIRE(mesh.Markers.size() == 1);
    }
    SECTION("Add a new marker to a non empty Bordercondition object"){
        mesh.AddMarkerElement(tag1, VTKId, nodeVec.data(), nodeVec.size());
        mesh.AddMarkerElement(tag2, VTKId, nodeVec.data(), nodeVec.size());
        REQUIRE(mesh.Markers.size() == 2);
    }
    SECTION("Add a new element to an existing marker"){
        mesh.AddMarkerElement(tag1, VTKId, nodeVec.data(), nodeVec.size());
        mesh.AddMarkerElement(tag2, VTKId, nodeVec.data(), nodeVec.size());
        mesh.AddMarkerElement(tag2, VTKId, nodeVec.data(), nodeVec.size());

        REQUIRE(mesh.Markers.size() == 2);

        REQUIRE(mesh.Markers["testTag1"].size() == 1);
        REQUIRE(mesh.Markers["testTag2"].size() == 2);
    }

}