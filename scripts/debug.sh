#!/bin/bash

export CXX="clang++"; 
cd ../cmake-build-debug
cmake .. -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug
make
./freeplanet --forceWindowed --forceUnlockedMouse </dev/null &>/dev/null &
