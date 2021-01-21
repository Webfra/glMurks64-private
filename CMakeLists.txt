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

    source/gfx/linmath.h

    source/gfx/graphics.h
    source/gfx/graphics.cpp
    source/gfx/texture.cpp
    source/gfx/texture.h
    source/gfx/rectangle.cpp
    source/gfx/rectangle.h
    source/gfx/text_screen.cpp
    source/gfx/text_screen.h
    source/gfx/text_screen_shaders.vs
    source/gfx/text_screen_shaders.gs
    source/gfx/text_screen_shaders.fs
    source/gfx/framebuffer.h

    )

#-----------------------------------------------------------------
target_include_directories( ${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source )
target_include_directories( ${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source/gfx )

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


