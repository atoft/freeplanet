#!/bin/bash

export CXX="clang++"; 
cd ../cmake-build-relwithdebinfo
cmake .. -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
make
./freeplanet --forceWindowed --forceUnlockedMouse </dev/null &>/dev/null &
