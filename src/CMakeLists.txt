add_subdirectory(parser)
add_subdirectory(partition)
add_subdirectory(mesh)
add_subdirectory(connectivity)
add_subdirectory(solver)
add_subdirectory(post)
add_subdirectory(parallelization)
add_subdirectory(utils)

# Setting flags
set(debug_flags -W -Wall -pedantic -O0 -g3)
set(release_flags -W -Wall -pedantic -O3 )




ADD_EXECUTABLE(E3D_PRE preprocessor.cpp )
target_include_directories(E3D_PRE PUBLIC ../include ../extern)

target_link_libraries(E3D_PRE PUBLIC Parser Mesh Utils Connectivity Partition)


ADD_EXECUTABLE(E3D_Solver Solver.cpp)
target_include_directories(E3D_Solver PUBLIC ../include)

target_link_libraries(E3D_Solver PUBLIC Parser Mesh Parallel_tools Utils Connectivity Solver)


target_link_libraries(E3D_Solver PUBLIC MPI::MPI_C)

# Post-traitement
ADD_EXECUTABLE(E3D_POST postprocessor.cpp)
target_include_directories(E3D_POST PUBLIC ../include ../extern)
target_link_libraries(E3D_POST PUBLIC Parser Mesh Utils Post Solver)



# INCLUDE MPI
find_package(MPI REQUIRED)
message(STATUS "Run: ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS} EXECUTABLE ${MPIEXEC_POSTFLAGS} ARGS")

install(TARGETS E3D_PRE
        CONFIGURATIONS Debug
        DESTINATION bin)

install(TARGETS E3D_PRE
        CONFIGURATIONS Release
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/include/)
