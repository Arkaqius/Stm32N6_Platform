@echo off
echo === Embedded Toolchain Setup for Windows ===

:: Install ARM GCC
where arm-none-eabi-gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] arm-none-eabi-gcc not found. Installing via winget...
    winget install -e --id Arm.GNUArmEmbeddedToolchain
) else (
    echo [✓] arm-none-eabi-gcc already installed.
)

:: Install CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] CMake not found. Installing via winget...
    winget install -e --id Kitware.CMake
) else (
    echo [✓] CMake already installed.
)

:: Install Git
where git >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] Git not found. Installing via winget...
    winget install -e --id Git.Git
) else (
    echo [✓] Git already installed.
)

:: Doxygen 
where doxygen >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] Git not found. Installing via winget...
    winget install -e --id Doxygen.Doxygen
) else (
    echo [✓] Git already installed.
)

:: Dot 
where dot >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] Git not found. Installing via winget...
    winget install -e --id Graphviz.Graphviz
) else (
    echo [✓] Git already installed.
)

echo [✓] Setup complete. Open "workspace" in VS Code
pause