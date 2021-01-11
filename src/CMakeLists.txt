# Setting flags
set(debug_flags -W -Wall -pedantic -O0 -g3)
set(release_flags -W -Wall -pedantic -O3 )

ADD_EXECUTABLE(Euler3D_APP main.cpp)


TARGET_COMPILE_OPTIONS(Euler3D_APP PUBLIC "$<$<CONFIG:DEBUG>:${debug_flags}>" )
TARGET_COMPILE_OPTIONS(Euler3D_APP PUBLIC "$<$<CONFIG:DEBUG>:${release_flags}>" )


INSTALL(TARGETS Euler3D_APP
        CONFIGURATIONS Debug
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)

INSTALL(TARGETS Euler3D_APP
        CONFIGURATIONS Release
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)