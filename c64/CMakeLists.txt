cmake_minimum_required(VERSION 3.5)

#========================================================================
project(c64 LANGUAGES CXX)
set ( target ${PROJECT_NAME} )

#========================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#========================================================================
add_library( ${target} OBJECT
        c64.cpp
        c64.h
        hex.h
    )

target_include_directories( ${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries( ${target} PRIVATE fake6502 )

#========================================================================
# End of file.
#========================================================================
