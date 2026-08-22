@echo off
setlocal EnableDelayedExpansion

set "REPO_ROOT=%~dp0.."
for %%I in ("%REPO_ROOT%") do set "REPO_ROOT=%%~fI"
set "BUILD_MODE="
set "BASH_EXE="

where wsl.exe >nul 2>nul
if not errorlevel 1 (
    wsl.exe -d Ubuntu -- true >nul 2>nul
    if not errorlevel 1 set "BUILD_MODE=WSL"
)

if not defined BUILD_MODE if exist "C:\cygwin64\bin\bash.exe" set "BASH_EXE=C:\cygwin64\bin\bash.exe"
if not defined BUILD_MODE if exist "C:\msys64\usr\bin\bash.exe" set "BASH_EXE=C:\msys64\usr\bin\bash.exe"
if not defined BUILD_MODE if defined BASH_EXE set "BUILD_MODE=BASH"

echo Building ZeroPoint...

if "!BUILD_MODE!"=="WSL" (
    pushd "!REPO_ROOT!\src"
    wsl.exe -d Ubuntu -- make all
    set "EXIT_CODE=!ERRORLEVEL!"
    popd
    goto finish
)

if "!BUILD_MODE!"=="BASH" (
    "!BASH_EXE!" -lc "cd \"$0/src\" && make all" "%REPO_ROOT%"
    set "EXIT_CODE=!ERRORLEVEL!"
    goto finish
)

echo No usable build environment was found.
echo WSL is installed but unhealthy, and no Cygwin/MSYS bash was found.
echo Fix WSL or install a local Unix toolchain for Windows.
set "EXIT_CODE=1"

:finish
if not "!EXIT_CODE!"=="0" (
    echo.
    echo Build failed with exit code !EXIT_CODE!.
    pause
    exit /b !EXIT_CODE!
)

echo.
echo Build completed.
pause
exit /b 0
