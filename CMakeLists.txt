cmake_minimum_required(VERSION 3.5)

project(glMurks64 LANGUAGES CXX)
set ( target ${PROJECT_NAME} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable( ${target}
    source/main.cpp
    source/utils.h
    source/mainwindow.h
    source/mainwindow.cpp
    source/graphics.h
    source/graphics.cpp
    source/texture.cpp
    source/texture.h
    source/text_screen.cpp
    source/text_screen.h
    text_screen.vert
    text_screen.vsh
    )

#-----------------------------------------------------------------
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions( ${target} PUBLIC -DDEBUG  )
endif()

#-----------------------------------------------------------------
add_subdirectory( glad )
target_link_libraries( ${target} PRIVATE glad )

#-----------------------------------------------------------------
# FindSDL2.cmake cloned from: https://github.com/tcbrindle/sdl2-cmake-scripts
list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/sdl2-cmake-scripts")
find_package(SDL2 REQUIRED )
target_link_libraries(${target} PRIVATE ${SDL2_LIBRARY} )
target_include_directories(${target} PRIVATE "${SDL2_INCLUDE_DIR}" )


