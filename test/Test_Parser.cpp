#include <catch2/catch.hpp>
#include "parser/SU2MeshParser.hpp"
#include <iostream>
#include "parser/SimConfig.hpp"

TEST_CASE("SU2MeshParser class test", "[parser]") {

    // Initialize SU2MeshParser class for testing
    std::string filename = "../../test/mesh/naca0012_9x9x5_unittest_mesh.su2";
    E3D::Parser::SU2MeshParser parser(filename);

    SECTION("Dimensions Parser") {
        // Test number of dimensions
        REQUIRE(parser.GetnDim() == 3);
    }

    SECTION("Interior Elements Parser") {

        const E3D::Parser::Element &firstElem = parser.GetVolumeElems().front();
        std::vector<int> firstElem_nodes = {735, 805, 835, 720, 757, 760, 0, 675};

        const E3D::Parser::Element &elem50 = parser.GetVolumeElems()[50];
        std::vector<int> elem50_nodes = {866, 867, 882, 881, 31, 32, 47, 46};

        const E3D::Parser::Element &elem666 = parser.GetVolumeElems()[666];
        std::vector<int> elem666_nodes = {354, 355, 370, 369, 579, 580, 595, 594};

        const E3D::Parser::Element &elem888 = parser.GetVolumeElems()[888];
        std::vector<int> elem888_nodes = {547, 548, 563, 562, 1232, 1233, 1248, 1247};

        const E3D::Parser::Element &lastElem = parser.GetVolumeElems().back();
        std::vector<int> lastElem_nodes = {674, 719, 739, 1119, 1359, 755, 740, 1134};

        // Number of interior elements
        REQUIRE(parser.GetVolumeElemCount() == 1024);

        //First element
        REQUIRE(firstElem.getVtkID() == 12);
        REQUIRE(firstElem.getElemNodes() == firstElem_nodes);

        REQUIRE(elem50.getVtkID() == 12);
        REQUIRE(elem50.getElemNodes() == elem50_nodes);

        REQUIRE(elem666.getVtkID() == 12);
        REQUIRE(elem666.getElemNodes() == elem666_nodes);

        REQUIRE(elem888.getVtkID() == 12);
        REQUIRE(elem888.getElemNodes() == elem888_nodes);

        REQUIRE(lastElem.getVtkID() == 12);
        REQUIRE(lastElem.getElemNodes() == lastElem_nodes);

    }

    SECTION("Nodes Parser") {

        const E3D::Parser::Node &firstNode = parser.GetPoints().front();
        const E3D::Parser::Node &node700 = parser.GetPoints()[700];
        const E3D::Parser::Node &lastNode = parser.GetPoints().back();

        REQUIRE(parser.GetPointsCount() == 1360);

        REQUIRE(firstNode.getX() == Approx(-0.025875383990));
        REQUIRE(firstNode.getY() == Approx(0.10638920220));
        REQUIRE(firstNode.getZ() == Approx(1.0000000000));

        REQUIRE(node700.getX() == Approx(-20.374994280));
        REQUIRE(node700.getY() == Approx(0.0000000000));
        REQUIRE(node700.getZ() == Approx(2.000000000));

        REQUIRE(lastNode.getX() == Approx(-92.187179570));
        REQUIRE(lastNode.getY() == Approx(-38.386707310));
        REQUIRE(lastNode.getZ() == Approx(4.00000000000));
    }

    SECTION("Boundary Conditions Parser") {
        std::vector<std::pair<std::string, int>> tags = {std::make_pair("farfield", 64),
                                                         std::make_pair("sym1", 256),
                                                         std::make_pair("sym2", 256),
                                                         std::make_pair("wall", 64)};

        std::vector<int> farfield_firstElem_nodes = {736, 1060, 1075, 737};
        std::vector<int> farfield_lastElem_nodes = {1119, 739, 740, 1134};

        std::vector<int> sym1_firstElem_nodes = {741, 1135, 820, 756};
        std::vector<int> sym1_lastElem_nodes = {1134, 740, 755, 1359};

        std::vector<int> sym2_firstElem_nodes = {735, 805, 835, 720};
        std::vector<int> sym2_lastElem_nodes = {1059, 734, 736, 1074};

        std::vector<int> wall_firstElem_nodes = {757, 760, 805, 735};
        std::vector<int> wall_lastElem_nodes = {834, 756, 759, 804};

        REQUIRE(parser.GetMarkersCount() == 4);

        REQUIRE(parser.GetBoundaryElemCount() == 640);

        REQUIRE(parser.GetTags() == tags);
        REQUIRE(parser.GetBoundaryElems().size() == 4);


        // Testing Interior VTKID vector
        REQUIRE(parser.GetInteriorElementVtkID().front() == 12);
        REQUIRE(parser.GetInteriorElementVtkID()[50] == 12 );
        REQUIRE(parser.GetInteriorElementVtkID().back() == 12);

        // Testing Interior face Count vector
        REQUIRE(parser.GetInteriorElementsFaceCount().front() == 6);
        REQUIRE(parser.GetInteriorElementsFaceCount()[50] == 6);
        REQUIRE(parser.GetInteriorElementsFaceCount().back() == 6);


        // First BC "farfield"
        REQUIRE(parser.GetBoundaryElems()[0].first == tags[0].first);
        REQUIRE(parser.GetBoundaryElems()[0].second.front().getElemNodes() == farfield_firstElem_nodes);
        REQUIRE(parser.GetBoundaryElems()[0].second.back().getElemNodes() == farfield_lastElem_nodes);

        // Second BC "sym1"
        REQUIRE(parser.GetBoundaryElems()[1].first == tags[1].first);
        REQUIRE(parser.GetBoundaryElems()[1].second.front().getElemNodes() == sym1_firstElem_nodes);
        REQUIRE(parser.GetBoundaryElems()[1].second.back().getElemNodes() == sym1_lastElem_nodes);

        // Third BC "sym2"
        REQUIRE(parser.GetBoundaryElems()[2].first == tags[2].first);
        REQUIRE(parser.GetBoundaryElems()[2].second.front().getElemNodes() == sym2_firstElem_nodes);
        REQUIRE(parser.GetBoundaryElems()[2].second.back().getElemNodes() == sym2_lastElem_nodes);

        // fourth BC "wall"
        REQUIRE(parser.GetBoundaryElems()[3].first == tags[3].first);
        REQUIRE(parser.GetBoundaryElems()[3].second.front().getElemNodes() == wall_firstElem_nodes);
        REQUIRE(parser.GetBoundaryElems()[3].second.back().getElemNodes() == wall_lastElem_nodes);

    }


}

TEST_CASE("SimConfig class test", "[parser]") {
    std::string ConfigFileName = "../../test/ConfigFiles/ConfigFIle_test.e3d";
    E3D::Parser::SimConfig config(ConfigFileName);

    // Testing mesh partitions file name
    std::vector<std::string> partitionFiles = {"../../test/mesh/StructuredBlock_8_p0.par",
                                               "../../test/mesh/StructuredBlock_8_p1.par",
                                               "../../test/mesh/StructuredBlock_8_p2.par",
                                               "../../test/mesh/StructuredBlock_8_p3.par"};
    std::vector<std::string> parsedPartitionFiles = config.getPartitionedMeshFiles();

    REQUIRE(config.getNumberPartitions() == 4);

    for (int i=0 ; i< config.getNumberPartitions(); i++){
        REQUIRE(parsedPartitionFiles[i] == partitionFiles[i]);
    }

    // Simulation parameters
    REQUIRE(config.getMach() == Approx(0.7346523151));
    REQUIRE(config.getVelocity() == 250);
    REQUIRE(config.getAoA() == 1);
    REQUIRE(config.getPressure() == 101325);
    REQUIRE(config.getTemperature() == 288.15);
    REQUIRE(config.getViscosity() == 1.853e-5);
    REQUIRE(config.getDensity() == 1.2886);
    REQUIRE(config.getGamma() == 1.4);
    REQUIRE(config.getGasConstant() == 287.058);
    REQUIRE(config.getSpecificHeat() == 1004.7);
    REQUIRE(config.getSolverScheme() == E3D::Parser::SimConfig::SolverScheme::AUSM);
    REQUIRE(config.getTemporalScheme() == E3D::Parser::SimConfig::TemporalScheme::RK5);
    REQUIRE(config.getCFL() == 3);
    REQUIRE(config.getMinResidual() == 1e-10);
    REQUIRE(config.getTecplotFile() == "output.dat");

    //





}
