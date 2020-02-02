@echo off

git clone --no-checkout https://github.com/microsoft/vcpkg.git
cd vcpkg

rem To update dependency versions, replace this with a newer hash from the vcpkg repo.
git checkout efba6833962764644225a3301c1a5fae293e9d87

call ./bootstrap-vcpkg.bat

call .\vcpkg.exe install glm:x64-windows
call .\vcpkg.exe install glew:x64-windows
call .\vcpkg.exe install sfml:x64-windows

echo .
echo .
echo .
echo Done. To build the project, open this folder as a CMake project in Visual Studio.
