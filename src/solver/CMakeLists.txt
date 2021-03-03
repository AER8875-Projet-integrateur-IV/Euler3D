add_library(Solver FlowField.cpp EulerSolver.cpp ../../include/solver/EulerSolver.hpp FarfieldBC.cpp ../../include/solver/FarfieldBC.h )
TARGET_INCLUDE_DIRECTORIES(Solver PUBLIC ${PROJECT_SOURCE_DIR}/include)

TARGET_LINK_LIBRARIES(Solver PUBLIC Parallel_tools Parser)