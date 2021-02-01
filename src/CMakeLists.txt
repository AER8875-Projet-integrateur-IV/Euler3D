add_subdirectory(parser)
add_subdirectory(partition)
add_subdirectory(mesh)



ADD_EXECUTABLE(Euler3D_APP main.cpp)
target_include_directories(Euler3D_APP PUBLIC ../include)

target_link_libraries(Euler3D_APP PUBLIC Parser Partition)

install(TARGETS Euler3D_APP
        CONFIGURATIONS Debug
        DESTINATION bin)

install(TARGETS Euler3D_APP
        CONFIGURATIONS Release
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/include/)