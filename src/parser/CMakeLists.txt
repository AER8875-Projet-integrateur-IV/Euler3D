
add_library(Parser SU2MeshParser.cpp SimConfig.cpp SU2PartitionParser.cpp ../../include/parser/SU2PartitionParser.hpp )
TARGET_INCLUDE_DIRECTORIES(Parser PUBLIC ${PROJECT_SOURCE_DIR}/include)

TARGET_COMPILE_OPTIONS(Parser PUBLIC "$<$<CONFIG:DEBUG>:${debug_flags}>" )
TARGET_COMPILE_OPTIONS(Parser PUBLIC "$<$<CONFIG:RELEASE>:${release_flags}>" )
