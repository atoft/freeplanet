# freeplanet
[![](https://github.com/atoft/freeplanet/workflows/CI/badge.svg)](https://github.com/atoft/freeplanet/actions?query=workflow%3ACI)

**Explore randomly generated worlds.**

A free and open source C++ 3D engine.

# Setup
## Linux
### Prerequisites
- The following must be installed with your package manager:
  - Clang
  - CMake
  - OpenGL
  - libudev
  - libx11
  - libxrandr
  
  On Ubuntu, you can use `sudo ./scripts/ubuntu-prerequisites.sh` to install these.

### Setup instructions
- Clone this repository.
- Run `./setup.sh`.
- Open the freeplanet folder as a CMake Project in your preferred IDE.
- Add the following as a CMake argument: `-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake`.

## Windows
### Prerequisites
- Windows 10 x64.
- Visual Studio 2019 with Clang. [How to install Clang.](https://devblogs.microsoft.com/cppblog/clang-llvm-support-in-visual-studio/)
- Git for Windows.

### Setup instructions
- Clone this repository.
- Run `.\setup.bat` from a PowerShell window.
- In Visual Studio, open the freeplanet folder as a CMake Project.
- Use the "Build" option on CMakeLists.txt to build and run in Visual Studio.


