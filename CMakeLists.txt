cmake_minimum_required(VERSION 2.6)

project(freeplanet LANGUAGES CXX)

if(WIN32)
    set(freeplanet_PLATFORM freeplanet_WIN32)
elseif(APPLE)
    set(freeplanet_PLATFORM freeplanet_APPLE)
else()
    set(freeplanet_PLATFORM freeplanet_LINUX)
endif()

set(freeplanet_VERSION_MAJOR 0)
set(freeplanet_VERSION_MINOR 1)
configure_file(
    "src/config.h.in"
    "${CMAKE_SOURCE_DIR}/src/config.h"
    )

# Automatically include all C++ files in the src directory.
file(GLOB_RECURSE sources      src/*.cpp src/*.h)

# On Windows, CMakeSettings.json should find clang for use, but the clang version for Windows takes different command line args.
if(WIN32)
	SET(CMAKE_CXX_FLAGS "/std:c++17")

	# Ignore warnings coming from the C runtime's usage in third party libraries:
	# https://docs.microsoft.com/en-us/cpp/c-runtime-library/security-features-in-the-crt?view=vs-2019
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

else()
	SET (CMAKE_CXX_COMPILER "clang++")
	set_source_files_properties(
        ${sources}
        PROPERTIES
		# Define compiler flags
		# Treat warnings as errors and enable all warnings, excluding unused parameters as there are some valid cases of that.
		# Use C++17 features.
        COMPILE_FLAGS "-Wextra -Wall -Werror -Wno-unused-parameter -std=c++17"
	)
endif()

set(EXECUTABLE_NAME "freeplanet")
add_executable(${EXECUTABLE_NAME} ${sources})

set(INCLUDE_DIRS ${INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}")
include_directories(${INCLUDE_DIRS})

# Assume we can find OpenGL whatever platform we're on.
find_package(OpenGL REQUIRED)

### Begin dependencies from vcpkg ###

find_package(GLEW REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GLEW::GLEW)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glm)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sfml-system sfml-window sfml-graphics)

if(WIN32)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE FLAC OpenAL OpenGL Vorbis)
else()
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE X11 FLAC UDev OpenAL)
endif()

### End dependencies from vcpkg ###

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${freeplanet_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${freeplanet_VERSION_MINOR}")
include(CPack)

# On Windows, the solution is able to build both Debug and Release.
# With Unix Makefiles on Linux, we need to CMake once per target, in the intended directory.
if(WIN32)
	file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/Debug)
	file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/Release)
else()
	file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
endif()

