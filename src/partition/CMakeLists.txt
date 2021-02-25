# STEPS TO CREATE A CMakeLists.txt

#   1. Write your library's name after NAME_LIBRARY. This name should be 
#      representative of the files your adding.
#      
#      examples:
#      set(NAME_LIBRARY Connectivity)
#      set(NAME_LIBRARY Solver)

set(NAME_LIBRARY Partition)

#   2. Write the source files you want to add to your library.
#      
#      examples:
#      set(SOURCE Partition.cpp SU2Writer.cpp)
#      set(NAME_LIBRARY SolverEuler.cpp)

set(SOURCE Partition.cpp SU2Writer.cpp PhysicalBCPartition)


#   3. Add the command add_subdirectory(NameOfThisDirectory) to the CMakeLists.txt
#      in the parent folder

set(DEPENDANCIES Utils Mesh Parser metis)

#   4. Add the command add_subdirectory(NameOfThisDirectory) to the CMakeLists.txt
#      in the parent folder

#   5. Add your library's name in the relevent 
#      target_link_libraries(Euler3D_APP PUBLIC ThisIsALibrary ThisTo ThisIsYourLibrairy) 
#
#      You need to link your library once for each executable that will need it

#       __   __                        _   _       _     _   _     _ 
#       \ \ / /   ___    _   _      __| | (_)   __| |   (_) | |_  | |
#        \ V /   / _ \  | | | |    / _` | | |  / _` |   | | | __| | |
#         | |   | (_) | | |_| |   | (_| | | | | (_| |   | | | |_  |_|
#         |_|    \___/   \__,_|    \__,_| |_|  \__,_|   |_|  \__| (_)


################################################################################
#              NOTHING SHOULD BE MODIFIED BELOW THIS LINE                      #
################################################################################

add_library(${NAME_LIBRARY} ${SOURCE})
TARGET_INCLUDE_DIRECTORIES(${NAME_LIBRARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
TARGET_INCLUDE_DIRECTORIES(${NAME_LIBRARY} PUBLIC ${PROJECT_SOURCE_DIR}/extern)
target_link_libraries(${NAME_LIBRARY} PUBLIC ${DEPENDANCIES})

TARGET_COMPILE_OPTIONS(${NAME_LIBRARY} PUBLIC "$<$<CONFIG:DEBUG>:${debug_flags}>" )
TARGET_COMPILE_OPTIONS(${NAME_LIBRARY} PUBLIC "$<$<CONFIG:RELEASE>:${release_flags}>" )