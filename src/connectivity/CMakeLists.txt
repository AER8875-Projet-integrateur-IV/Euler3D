
add_library(Connectivity Connectivity.cpp)

target_link_libraries(Connectivity PUBLIC Mesh)

TARGET_INCLUDE_DIRECTORIES(Connectivity PUBLIC ../../include)
