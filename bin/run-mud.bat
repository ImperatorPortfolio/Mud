@echo off
setlocal EnableDelayedExpansion

set "PORT=%~1"
if "%PORT%"=="" set "PORT=4020"
set "REPO_ROOT=%~dp0.."
for %%I in ("%REPO_ROOT%") do set "REPO_ROOT=%%~fI"
set "MUD_HOST=127.0.0.1"
set "USE_WSL=0"

where wsl.exe >nul 2>nul
if not errorlevel 1 (
    wsl.exe -d Ubuntu -- true >nul 2>nul
    if not errorlevel 1 (
        for /f "tokens=1" %%I in ('wsl.exe -d Ubuntu -- hostname -I') do if not defined MUD_HOST_FROM_WSL set "MUD_HOST_FROM_WSL=%%I"
        set "USE_WSL=1"
    )
)

if "!USE_WSL!"=="1" (
    if defined MUD_HOST_FROM_WSL set "MUD_HOST=!MUD_HOST_FROM_WSL!"
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

    echo Streaming the active log below; output is also written to the log directory.
    echo Press Ctrl+C to stop the server, or use shutdown mud now in-game.
    echo.
    pushd "!REPO_ROOT!\src"
    wsl.exe -d Ubuntu -- env ZEROPOINT_STREAM_LOG=1 bash ./startup.sh %PORT%
    set "EXIT_CODE=!ERRORLEVEL!"
    popd
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
