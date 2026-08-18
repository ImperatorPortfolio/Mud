@echo off
setlocal

set "PORT=%~1"
if "%PORT%"=="" set "PORT=4020"
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

for /f "tokens=1" %%I in ('wsl.exe -d Ubuntu -- hostname -I') do set "MUD_HOST=%%I"
if "%MUD_HOST%"=="" (
    echo Failed to determine the WSL network address.
    pause
    exit /b 1
)

echo Starting SWRFUSS on port %PORT%...
echo Connect Mudlet to %MUD_HOST%:%PORT% with Secure disabled.
echo Streaming the active log file below.
echo Press Ctrl+C in this window to stop monitoring.
echo.
wsl.exe -d Ubuntu -- bash -lc "cd \"%WSL_ROOT%/src\" && ./startup.sh %PORT% & STARTUP_PID=\$!; sleep 2; LOGFILE=\$(ls -1t ../log/*.log 2>/dev/null | head -n 1); if [ -z \"\$LOGFILE\" ]; then echo 'No log file was created. Build may be missing or startup failed early.'; wait \$STARTUP_PID; exit \$?; fi; echo Monitoring \$LOGFILE; echo; tail -n 50 -f \"\$LOGFILE\"; wait \$STARTUP_PID"
set "EXIT_CODE=%ERRORLEVEL%"

echo.
echo SWRFUSS exited with code %EXIT_CODE%.
pause
exit /b %EXIT_CODE%
