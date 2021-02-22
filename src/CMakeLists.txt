add_subdirectory(parser)
add_subdirectory(mesh)

add_subdirectory(parallelization)


add_subdirectory(utils)
<<<<<<< HEAD
add_subdirectory(solver)
=======
add_subdirectory(connectivity)
>>>>>>> c61d077ea18a1bc8325086394072324464a6c7a8



ADD_EXECUTABLE(E3D_PRE preprocessor.cpp )
target_include_directories(E3D_PRE PUBLIC ../include ../extern)
target_link_libraries(E3D_PRE PUBLIC Parser Mesh Utils Connectivity)

ADD_EXECUTABLE(E3D_Solver Solver.cpp)
target_include_directories(E3D_Solver PUBLIC ../include)
<<<<<<< HEAD
target_link_libraries(E3D_Solver PUBLIC Parser Mesh Utils Solver)
=======
>>>>>>> c61d077ea18a1bc8325086394072324464a6c7a8

target_link_libraries(E3D_Solver PUBLIC Parser Mesh Parallel_tools Utils Connectivity)


target_link_libraries(E3D_Solver PUBLIC MPI::MPI_CXX)

# INCLUDE MPI
find_package(MPI REQUIRED)
message(STATUS "Run: ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS} EXECUTABLE ${MPIEXEC_POSTFLAGS} ARGS")

install(TARGETS E3D_PRE
        CONFIGURATIONS Debug
        DESTINATION bin)

install(TARGETS E3D_PRE
        CONFIGURATIONS Release
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/include/)