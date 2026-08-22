@echo off
setlocal EnableDelayedExpansion

set "PORT=%~1"
if "%PORT%"=="" set "PORT=4020"
set "REPO_ROOT=%~dp0.."
for %%I in ("%REPO_ROOT%") do set "REPO_ROOT=%%~fI"
set "MUD_HOST=127.0.0.1"
set "USE_WSL=0"
set "WSL_ROOT="

where wsl.exe >nul 2>nul
if not errorlevel 1 (
    wsl.exe -d Ubuntu -- true >nul 2>nul
    if not errorlevel 1 (
        for /f "usebackq delims=" %%I in (`wsl.exe wslpath "%REPO_ROOT%"`) do set "WSL_ROOT=%%I"
        for /f "tokens=1" %%I in ('wsl.exe -d Ubuntu -- hostname -I') do if not defined MUD_HOST_FROM_WSL set "MUD_HOST_FROM_WSL=%%I"
    )
)

if defined WSL_ROOT (
    if defined MUD_HOST_FROM_WSL set "MUD_HOST=!MUD_HOST_FROM_WSL!"
    set "USE_WSL=1"
)

echo Starting ZeroPoint on port %PORT%...
echo Connect Mudlet to !MUD_HOST!:%PORT% with Secure disabled.

if "!USE_WSL!"=="1" (
    wsl.exe -d Ubuntu -- bash -lc "ss -ltn \"sport = :%PORT%\" | grep -q LISTEN"
    if !ERRORLEVEL! EQU 0 (
        echo.
        echo ZeroPoint is already running on port %PORT%.
        echo Stop the existing instance before starting another one.
        set "EXIT_CODE=2"
        goto finish
    )

    echo Server output is written to the log directory.
    echo Press Ctrl+C to stop the server, or use shutdown mud now in-game.
    echo.
    wsl.exe -d Ubuntu -- bash -lc "cd \"%WSL_ROOT%/src\" && exec ./startup.sh %PORT%"
    set "EXIT_CODE=!ERRORLEVEL!"
    goto finish
)

:native_run
echo WSL is unavailable or unhealthy. Falling back to the native Windows binary.
echo Streaming ZeroPoint output below.
echo Press Ctrl+C in this window to stop monitoring.
echo.
pushd "%REPO_ROOT%\area"
if exist shutdown.txt del /q shutdown.txt
"%REPO_ROOT%\src\ZeroPoint.exe" "%PORT%"
set "EXIT_CODE=!ERRORLEVEL!"
popd

:finish
echo.
echo ZeroPoint exited with code !EXIT_CODE!.
pause
exit /b !EXIT_CODE!