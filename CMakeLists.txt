CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

project(Euler3D LANGUAGES CXX VERSION "0.1.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_compile_options_config(<CONFIG> <option> ...)
function(add_compile_options_config CONFIG)
    foreach(opt ${ARGN})
        add_compile_options("$<$<CONFIG:${CONFIG}>:${opt}>")
    endforeach()
endfunction()

# Add static analysis tool and sanitizers when needed ( "-fanalyzer", "-fsanitize=leak" ...)

# add_link_options(-fsanitize=leak,undefined,address)
# ---  Add same flag to compile options ----

add_compile_options_config(RELEASE "-Wall" "-Wextra" "-Ofast" "-pedantic")
add_compile_options_config(DEBUG "-Wall" "-Wextra" "-g3" "-pedantic")

ADD_SUBDIRECTORY(src)

#include tests
option(PACKAGE_TESTS "Build the tests" OFF)

#include MPI



IF(PACKAGE_TESTS)
    ADD_SUBDIRECTORY(test)
ENDIF()