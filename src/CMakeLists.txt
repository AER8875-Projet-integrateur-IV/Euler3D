add_subdirectory(parser)
add_subdirectory(mesh)
add_subdirectory(utils)
add_subdirectory(solver)



ADD_EXECUTABLE(E3D_PRE preprocessor.cpp )
target_include_directories(E3D_PRE PUBLIC ../include ../extern)
target_link_libraries(E3D_PRE PUBLIC Parser Mesh Utils)

ADD_EXECUTABLE(E3D_Solver Solver.cpp)
target_include_directories(E3D_Solver PUBLIC ../include)
target_link_libraries(E3D_Solver PUBLIC Parser Mesh Utils Solver)


install(TARGETS E3D_PRE
        CONFIGURATIONS Debug
        DESTINATION bin)

install(TARGETS E3D_PRE
        CONFIGURATIONS Release
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/include/)