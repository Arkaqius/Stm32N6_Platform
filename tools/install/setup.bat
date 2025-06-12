@echo off
echo === Embedded Toolchain Setup for Windows ===

:: Install ARM GCC
where arm-none-eabi-gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] arm-none-eabi-gcc not found. Installing via winget...
    winget install -e --id Arm.GnuArmEmbeddedToolchain
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

:: Dot 
where make >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] Make not found. Installing via winget...
    winget install -e --id GnuWin32.Make
) else (
    echo Make already installed.
)

:: Doxygen 
where doxygen >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] Doxygen not found. Installing via winget...
    winget install -e --id DimitriVanHeesch.Doxygen
) else (
    echo [✓] Doxygen already installed.
)

:: Dot 
where dot >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] Dot (Graphiv) not found. Installing via winget...
    winget install -e --id Graphviz.Graphviz
) else (
    echo [✓] Dot (Graphiv) already installed.
)

echo [✓] Setup complete. Open "workspace" in VS Code
pause
