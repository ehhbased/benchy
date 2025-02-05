@echo off
echo Building Benchy...

:: Check if build directory exists, create if not
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

:: Navigate to build directory
cd build

:: Run CMake
echo Configuring with CMake...
cmake ../

:: Build the project
echo Building project...
cmake --build . --config Release

:: Check if build was successful
if %ERRORLEVEL% == 0 (
    echo.
    echo Build completed successfully!
    echo The executable can be found in: build\Release\benchy_main.exe
) else (
    echo.
    echo Build failed! Please check the error messages above.
)

:: Keep the window open if there were errors
if %ERRORLEVEL% NEQ 0 (
    pause
) 