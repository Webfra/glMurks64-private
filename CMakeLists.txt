cmake_minimum_required(VERSION 3.5)

#========================================================================
project(glMurks64 LANGUAGES CXX)
set ( target ${PROJECT_NAME} )

#========================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#========================================================================
set( src ${CMAKE_CURRENT_SOURCE_DIR}/source )
set( gfx ${CMAKE_CURRENT_SOURCE_DIR}/source/gfx )

#========================================================================
add_executable( ${target}

    ${src}/main.cpp
    ${src}/utils.h
    ${src}/mainwindow.h
    ${src}/mainwindow.cpp

#    ${gfx}/linmath.h

    ${gfx}/gfx_utils.cpp
    ${gfx}/gfx_utils.h
    ${gfx}/graphics.h
    ${gfx}/graphics.cpp
    ${gfx}/texture.cpp
    ${gfx}/texture.h
    ${gfx}/shader.cpp
    ${gfx}/shader.h
    ${gfx}/rectangle.cpp
    ${gfx}/rectangle.h
    ${gfx}/text_screen.cpp
    ${gfx}/text_screen.h
    ${gfx}/framebuffer.cpp
    ${gfx}/framebuffer.h

    )

#========================================================================
target_include_directories( ${target} PRIVATE ${src} )
target_include_directories( ${target} PRIVATE ${gfx} )

#========================================================================
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions( ${target} PUBLIC -DDEBUG  )
endif()

#========================================================================
add_subdirectory( glad )
target_link_libraries( ${target} PRIVATE glad )

#========================================================================
# FindSDL2.cmake cloned from: https://github.com/tcbrindle/sdl2-cmake-scripts
list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/sdl2-cmake-scripts")
find_package(SDL2 REQUIRED )
target_link_libraries(${target} PRIVATE ${SDL2_LIBRARY} )
target_include_directories(${target} PRIVATE "${SDL2_INCLUDE_DIR}" )

#========================================================================
# Add GLM library.
add_subdirectory( glm/glm )
target_link_libraries( ${target} PRIVATE glm )

#========================================================================
# End of file.
#========================================================================

