@echo off
setlocal

set "REPO_ROOT=%~dp0.."
for %%I in ("%REPO_ROOT%") do set "REPO_ROOT=%%~fI"

where wsl.exe >nul 2>nul
if errorlevel 1 (
    echo WSL is not installed or not on PATH.
    echo Install WSL and Ubuntu first, then rerun this script.
    pause
    exit /b 1
)

for /f "usebackq delims=" %%I in (`wsl.exe wslpath "%REPO_ROOT%"`) do set "WSL_ROOT=%%I"
if "%WSL_ROOT%"=="" (
    echo Failed to convert the repository path to a WSL path.
    pause
    exit /b 1
)

echo Building SWRFUSS...
wsl.exe -d Ubuntu -- bash -lc "cd \"%WSL_ROOT%/src\" && make"
set "EXIT_CODE=%ERRORLEVEL%"

if not "%EXIT_CODE%"=="0" (
    echo.
    echo Build failed with exit code %EXIT_CODE%.
    pause
    exit /b %EXIT_CODE%
)

echo.
echo Build completed.
pause
exit /b 0
