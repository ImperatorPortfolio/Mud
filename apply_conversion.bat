@echo off
setlocal
pushd "%~dp0" || (
    echo Unable to open the conversion directory: %~dp0
    pause
    exit /b 1
)

echo Local Sphere basic conversion
echo.

where py >nul 2>nul
if not errorlevel 1 (
    py -3 apply_conversion.py
) else (
    where python >nul 2>nul
    if errorlevel 1 (
        echo Python 3 was not found. Install Python 3 and enable its launcher or PATH entry.
        popd
        pause
        exit /b 1
    )
    python apply_conversion.py
)

if errorlevel 1 (
    echo.
    echo Conversion failed. Review the error above.
    popd
    pause
    exit /b 1
)
echo.
echo Conversion finished.
popd
pause
