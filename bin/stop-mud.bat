@echo off
setlocal EnableDelayedExpansion

set "PORT=%~1"
if "%PORT%"=="" set "PORT=4020"
set "REPO_ROOT=%~dp0.."
for %%I in ("%REPO_ROOT%") do set "REPO_ROOT=%%~fI"
set "WSL_ROOT="

where wsl.exe >nul 2>nul
if errorlevel 1 goto no_wsl

wsl.exe -d Ubuntu -- true >nul 2>nul
if errorlevel 1 goto no_wsl

for /f "usebackq delims=" %%I in (`wsl.exe -d Ubuntu -- wslpath "%REPO_ROOT%"`) do set "WSL_ROOT=%%I"
if not defined WSL_ROOT goto no_wsl

wsl.exe -d Ubuntu -- bash "!WSL_ROOT!/src/stop.sh" %PORT%
set "EXIT_CODE=!ERRORLEVEL!"
goto finish

:no_wsl
echo WSL Ubuntu is required to stop this Linux ZeroPoint instance.
set "EXIT_CODE=1"

:finish
exit /b !EXIT_CODE!