
add_library(Mesh Mesh.cpp)
TARGET_INCLUDE_DIRECTORIES(Mesh PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(Mesh PUBLIC Parser)
TARGET_COMPILE_OPTIONS(Mesh PUBLIC "$<$<CONFIG:DEBUG>:${debug_flags}>" )
TARGET_COMPILE_OPTIONS(Mesh PUBLIC "$<$<CONFIG:RELEASE>:${release_flags}>" )
