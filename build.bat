@echo off
REM EventChains Cross-Platform Build Script for Windows
REM Builds for Linux and Windows using Docker

setlocal enabledelayedexpansion

set IMAGE_NAME=eventchains-builder
set CONTAINER_NAME=eventchains-build-temp
set OUTPUT_DIR=build-output

echo.
echo ╔════════════════════════════════════════════════════════════╗
echo ║                                                            ║
echo ║        EventChains Cross-Platform Build System             ║
echo ║           Linux (native) + Windows (MinGW)                 ║
echo ║                                                            ║
echo ╚════════════════════════════════════════════════════════════╝
echo.

REM Check if Docker is available
docker --version >nul 2>&1
if errorlevel 1 (
    echo ✗ Error: Docker is not installed or not in PATH
    echo   Please install Docker Desktop from https://www.docker.com/
    exit /b 1
)
echo ✓ Docker is available

REM Clean previous build artifacts
echo.
echo → Cleaning previous build artifacts...
if exist "%OUTPUT_DIR%" rmdir /s /q "%OUTPUT_DIR%"
mkdir "%OUTPUT_DIR%"
echo ✓ Clean complete

REM Build Docker image
echo.
echo → Building Docker image: %IMAGE_NAME%
echo   This includes:
echo     • Ubuntu 22.04 base
echo     • GCC for Linux builds
echo     • MinGW-w64 for Windows cross-compilation
echo     • CMake build system
echo.

docker build -t %IMAGE_NAME% .
if errorlevel 1 (
    echo ✗ Error: Docker build failed
    exit /b 1
)
echo ✓ Docker image built successfully

REM Create container to extract binaries
echo.
echo → Creating temporary container to extract binaries...

REM Remove old container if it exists
docker rm %CONTAINER_NAME% >nul 2>&1

REM Create container
docker create --name %CONTAINER_NAME% %IMAGE_NAME% >nul
if errorlevel 1 (
    echo ✗ Error: Failed to create container
    exit /b 1
)
echo ✓ Container created

REM Copy binaries from container to host
echo.
echo → Extracting compiled binaries...

docker cp %CONTAINER_NAME%:/binaries/. %OUTPUT_DIR%/
if errorlevel 1 (
    echo ✗ Error: Failed to extract binaries
    docker rm %CONTAINER_NAME%
    exit /b 1
)
echo ✓ Binaries extracted to: %OUTPUT_DIR%

REM Clean up container
docker rm %CONTAINER_NAME% >nul
echo ✓ Temporary container removed

REM Display build artifacts
echo.
echo ╔════════════════════════════════════════════════════════════╗
echo ║                    Build Complete!                         ║
echo ╚════════════════════════════════════════════════════════════╝
echo.
echo Build artifacts are available in: %OUTPUT_DIR%
echo.

if exist "%OUTPUT_DIR%\linux\eventchain_test" (
    echo   ✓ Linux binary:   %OUTPUT_DIR%\linux\eventchain_test
) else (
    echo   ✗ Linux binary:   NOT FOUND
)

if exist "%OUTPUT_DIR%\windows\eventchain_test.exe" (
    echo   ✓ Windows binary: %OUTPUT_DIR%\windows\eventchain_test.exe
) else (
    echo   ✗ Windows binary: NOT FOUND
)

if exist "%OUTPUT_DIR%\README.md" (
    echo   ✓ Documentation:  %OUTPUT_DIR%\README.md
)

if exist "%OUTPUT_DIR%\eventchains.h" (
    echo   ✓ Header file:    %OUTPUT_DIR%\eventchains.h
)

echo.
echo ════════════════════════════════════════════════════════════
echo.
echo Usage:
echo   Windows: .\%OUTPUT_DIR%\windows\eventchain_test.exe [iterations]
echo   Linux:   ./%OUTPUT_DIR%/linux/eventchain_test [iterations]
echo.
echo Options:
echo   [iterations]  Number of benchmark iterations (default: 10000)
echo.
echo Build process complete!
echo.

endlocal
