
add_library(Connectivity Connectivity.cpp)
target_link_libraries(Connectivity PUBLIC Mesh)

ADD_EXECUTABLE(Connectivity_APP exemple.cpp)
target_include_directories(Connectivity_APP PUBLIC ../include)

target_link_libraries(Connectivity_APP PUBLIC Parser Mesh Connectivity)

TARGET_INCLUDE_DIRECTORIES(Connectivity PUBLIC ../../include)
