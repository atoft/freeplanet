cmake_minimum_required(VERSION 2.6)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose type of build (Debug or Release)" FORCE)
endif()

project(onua)

if(WIN32)
    set(freeplanet_PLATFORM freeplanet_WIN32)
elseif(APPLE)
    set(onua_PLATFORM onua_APPLE)
else()
    set(freeplanet_PLATFORM freeplanet_LINUX)
endif()

set(onua_VERSION_MAJOR 1)
set(onua_VERSION_MINOR 0)
configure_file(
    "src/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
    )

# Automatically include all C++ files in the src directory.
file(GLOB_RECURSE sources      src/*.cpp src/*.h)

# Define compiler flags
# Treat warnings as errors and enable all warnings, excluding unused parameters as there are some valid cases of that.
# Use C++17 features.
set_source_files_properties(
        ${sources}
        PROPERTIES
        COMPILE_FLAGS "-Wextra -Wall -Werror -Wno-unused-parameter -std=c++17"
)

SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-pthread -lX11 -lsfml-graphics -lsfml-window -lsfml-system -Wno-unused-command-line-argument")

set(EXECUTABLE_NAME "onua")
add_executable(${EXECUTABLE_NAME} ${sources})

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
# Find any version 2.x of SFML
find_package(SFML 2 REQUIRED window system graphics network audio)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
#find_package(Boost REQUIRED)
#find_package(ASSIMP REQUIRED)
#set(BUILD_ASSIMP_TOOLS  ON)

# Set the directories to search for #include. Need to add the roots of any third party libs here to make their includes work
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}")

# Dynamic linking for libraries.
set(LIBS ${LIBS} ${OPENGL_LIBRARY} ${GLEW_LIBRARIES}  X11)
include_directories(${INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${LIBS})

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)

include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${freeplanet_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${freeplanet_VERSION_MINOR}")
include(CPack)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

