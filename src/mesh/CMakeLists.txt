
add_library(Mesh Mesh.cpp Metrics.cpp)

TARGET_INCLUDE_DIRECTORIES(Mesh PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(Mesh PUBLIC Parser)

