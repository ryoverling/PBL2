@echo off
setlocal
cd /d "%~dp0"

where cmake >nul 2>nul
if errorlevel 1 if exist "C:\Qt\Tools\CMake_64\bin\cmake.exe" set "PATH=C:\Qt\Tools\CMake_64\bin;%PATH%"
where ninja >nul 2>nul
if errorlevel 1 if exist "C:\Qt\Tools\Ninja\ninja.exe" set "PATH=C:\Qt\Tools\Ninja;%PATH%"

echo Building MoneyManager C++ core...
cmake -S . -B build/core -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1

cmake --build build/core --config Release
if errorlevel 1 exit /b 1

ctest --test-dir build/core -C Release --output-on-failure
if errorlevel 1 exit /b 1

echo Core build and tests passed. No user interface is included.
