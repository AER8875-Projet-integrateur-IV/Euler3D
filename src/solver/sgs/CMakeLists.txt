add_library(Sgs bound.cpp
                cal_Q.cpp
                cal_RHS.cpp
                cal_for_lusgs.cpp
                eigenValues.cpp
                fvs.cpp
                inner_ite.cpp
                muscl.cpp
                parameters.cpp
                setup.cpp
                utils.cpp
                yacobi_A.cpp)
                
TARGET_INCLUDE_DIRECTORIES(Sgs PUBLIC ${PROJECT_SOURCE_DIR}/include)
TARGET_INCLUDE_DIRECTORIES(Sgs PUBLIC ${PROJECT_SOURCE_DIR}/extern)
