@echo off
setlocal
cd /d "%~dp0"
if not exist "build\qt\MoneyManager.exe" (
    call build.bat
    if errorlevel 1 exit /b 1
)
start "" "build\qt\MoneyManager.exe"
