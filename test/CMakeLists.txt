#Set up of the Catch2 Testing framework
#

#Add executables to this variable
set(TEST_SOURCE
    CatchMain.cpp 
    Test_Catch.cpp 
    Test_Parser.cpp
    Test_SU2Writer.cpp
    Test_SU2Mesh.cpp
    Test_Element.cpp
    Test_PhysicalBCPartition.cpp
    Test_Connectivity.cpp
)

# Add subdirectories


        ############################################## #
#Nothing should have to be changed from here
        ############################################## #message(STATUS "Tests are included")

#link Catch2 library
add_subdirectory(extern/Catch2)
add_executable(tests ${TEST_SOURCE})


target_link_libraries(tests Catch2::Catch2 
                            Mesh
                            Parser 
                            Connectivity
                            Parallel_tools
                            Partition)


target_link_libraries(tests MPI::MPI_CXX)
TARGET_INCLUDE_DIRECTORIES(tests PUBLIC ${PROJECT_SOURCE_DIR}/include)
TARGET_INCLUDE_DIRECTORIES(tests PUBLIC ${PROJECT_SOURCE_DIR}/extern)
find_package(MPI REQUIRED)

# add Catch2 cmake scripts to module path to allow automatic
# test discovery
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/test/extern/Catch2/contrib/")
message(STATUS "CMAKE_MODULE_PATH = " ${CMAKE_MODULE_PATH})
include(CTest)
include(Catch)
catch_discover_tests(tests)
