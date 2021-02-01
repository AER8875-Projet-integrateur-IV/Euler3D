add_subdirectory(parser)
add_subdirectory(mesh)
add_subdirectory(post)

# Setting flags
set(debug_flags -W -Wall -pedantic -O0 -g3)
set(release_flags -W -Wall -pedantic -O3 )




ADD_EXECUTABLE(Euler3D_APP main.cpp)
target_include_directories(Euler3D_APP PUBLIC ../include)
target_link_libraries(Euler3D_APP PUBLIC Parser Mesh Post)

TARGET_COMPILE_OPTIONS(Euler3D_APP PUBLIC "$<$<CONFIG:DEBUG>:${debug_flags}>" )
TARGET_COMPILE_OPTIONS(Euler3D_APP PUBLIC "$<$<CONFIG:DEBUG>:${release_flags}>" )
