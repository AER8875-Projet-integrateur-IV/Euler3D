add_library(Solver FlowField.cpp EulerSolver.cpp PhysicalBC.cpp )
TARGET_INCLUDE_DIRECTORIES(Solver PUBLIC ${PROJECT_SOURCE_DIR}/include)

TARGET_LINK_LIBRARIES(Solver PUBLIC Parallel_tools Parser)