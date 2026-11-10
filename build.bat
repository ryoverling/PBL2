@echo off
setlocal
cd /d "%~dp0"

where cmake >nul 2>nul
if errorlevel 1 if exist "C:\Qt\Tools\CMake_64\bin\cmake.exe" set "PATH=C:\Qt\Tools\CMake_64\bin;%PATH%"
where ninja >nul 2>nul
if errorlevel 1 if exist "C:\Qt\Tools\Ninja\ninja.exe" set "PATH=C:\Qt\Tools\Ninja;%PATH%"

if not defined QT_MINGW_ROOT set "QT_MINGW_ROOT=C:\Qt\Tools\mingw1310_64"
if not defined QT_ROOT set "QT_ROOT=C:\Qt\6.12.0\mingw_64"

if /i "%~1"=="test" goto test
if /i "%~1"=="core" goto core

:: Build GUI (khi da co Qt)
if not exist "%QT_ROOT%\bin\windeployqt.exe" (
    echo Qt kit not found. Set QT_ROOT to your Qt MinGW kit directory.
    exit /b 1
)
if not exist "%QT_MINGW_ROOT%\bin\g++.exe" (
    echo Compiler not found. Set QT_MINGW_ROOT to the MinGW matching your Qt kit.
    exit /b 1
)
set "PATH=%QT_ROOT%\bin;%QT_MINGW_ROOT%\bin;%PATH%"
set "QT_PLUGIN_PATH=%QT_ROOT%\plugins"
echo Dang bien dich ung dung MoneyManager GUI...
cmake -S . -B build/qt -G Ninja -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=%QT_ROOT%" "-DCMAKE_CXX_COMPILER=%QT_MINGW_ROOT%/bin/g++.exe"
if errorlevel 1 exit /b 1
cmake --build build/qt --target MoneyManager
if errorlevel 1 exit /b 1
"%QT_ROOT%\bin\windeployqt.exe" --release --no-translations --compiler-runtime build\qt\MoneyManager.exe
if errorlevel 1 exit /b 1
echo San sang: build\qt\MoneyManager.exe
echo Chay run.bat de mo ung dung.
exit /b 0

:core
set "PATH=%QT_MINGW_ROOT%\bin;%PATH%"
echo Bien dich thu vien core (C++ thuan)...
cmake -S . -B build/core -G Ninja -DCMAKE_BUILD_TYPE=Release "-DCMAKE_CXX_COMPILER=%QT_MINGW_ROOT%/bin/g++.exe"
if errorlevel 1 exit /b 1
cmake --build build/core --target core
if errorlevel 1 exit /b 1
echo Thu vien core bien dich thanh cong!
exit /b 0

:test
set "PATH=%QT_MINGW_ROOT%\bin;%PATH%"
echo Bien dich va chay chuong trinh test console_test...
cmake -S . -B build/test -G Ninja -DCMAKE_BUILD_TYPE=Release "-DCMAKE_CXX_COMPILER=%QT_MINGW_ROOT%/bin/g++.exe"
if errorlevel 1 exit /b 1
cmake --build build/test --target console_test
if errorlevel 1 exit /b 1
echo.
build\test\console_test.exe
if errorlevel 1 exit /b 1
exit /b 0
