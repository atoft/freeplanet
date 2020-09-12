#!/bin/bash
# 
# Generate compile_commands.json and copy it to the root directory.
# Enables clangd to be used as a language server for the project in
# in IDE or Vim.

mkdir cmake-build-release
cd cmake-build-release

export CXX="clang++"; cmake .. -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release

cp compile_commands.json ..
