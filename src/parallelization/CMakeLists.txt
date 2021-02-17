add_library(Parallel_tools MPIHandler.cpp)
find_package(MPI REQUIRED)
target_link_libraries(Parallel_tools  PUBLIC MPI::MPI_CXX)
TARGET_INCLUDE_DIRECTORIES(Parallel_tools PUBLIC ${PROJECT_SOURCE_DIR}/include)