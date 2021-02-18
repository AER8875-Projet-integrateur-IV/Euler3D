add_subdirectory(parser)
add_subdirectory(mesh)
add_subdirectory(post)
add_subdirectory(utils)

# Setting flags
set(debug_flags -W -Wall -pedantic -O0 -g3)
set(release_flags -W -Wall -pedantic -O3 )



ADD_EXECUTABLE(E3D_POST postprocess.cpp)
target_include_directories(Euler3D_APP PUBLIC ../include ../extern)
target_link_libraries(Euler3D_APP PUBLIC Parser Mesh Utils Post)

TARGET_COMPILE_OPTIONS(E3D_POST PUBLIC "$<$<CONFIG:DEBUG>:${debug_flags}>" )
TARGET_COMPILE_OPTIONS(E3D_POST  PUBLIC "$<$<CONFIG:DEBUG>:${release_flags}>" )
