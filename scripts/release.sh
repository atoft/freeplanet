#!/bin/bash

export CXX="clang++";
mkdir ../cmake-build-release
cd ../cmake-build-release
cmake .. -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
make && ./freeplanet </dev/null &>/dev/null &
