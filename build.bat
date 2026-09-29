@echo off
setlocal

echo ==========================================
echo        MoneyManager - Qt 6 Build
echo ==========================================

if not exist build mkdir build

cmake -S . -B build
if errorlevel 1 (
    echo.
    echo CMake khong tim thay Qt.
    echo Neu ban da cai Qt, hay cau hinh CMAKE_PREFIX_PATH.
    echo Vi du:
    echo cmake -S . -B build -DCMAKE_PREFIX_PATH=C:\Qt\6.8.0\mingw_64
    pause
    exit /b 1
)

cmake --build build --config Release
if errorlevel 1 (
    echo Build that bai.
    pause
    exit /b 1
)

echo.
echo Build thanh cong.
echo File chay nam trong thu muc build.
pause
