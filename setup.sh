#!/bin/bash

git clone --no-checkout https://github.com/microsoft/vcpkg.git
cd vcpkg

# To update dependency versions, replace this with a newer hash from the vcpkg repo.
git checkout efba6833962764644225a3301c1a5fae293e9d87

./bootstrap-vcpkg.sh

./vcpkg install glm:x64-linux
./vcpkg install glew:x64-linux
./vcpkg install sfml:x64-linux

echo .
echo .
echo .
echo Done. To build the project, open this folder as a CMake project.
