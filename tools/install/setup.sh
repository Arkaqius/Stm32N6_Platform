#!/bin/bash

echo "=== Embedded Toolchain Setup for Linux ==="

# Function to check and install a package
install_if_missing() {
    local pkg_name=$1
    local cmd_check=$2

    if ! command -v "$cmd_check" &> /dev/null; then
        echo "[!] $pkg_name not found. Installing..."
        sudo apt-get update
        sudo apt-get install -y "$pkg_name"
    else
        echo "[✓] $pkg_name already installed."
    fi
}

# Install required tools
install_if_missing "gcc-arm-none-eabi" "arm-none-eabi-gcc"
install_if_missing "cmake" "cmake"
install_if_missing "git" "git"
install_if_missing "doxygen" "doxygen"
install_if_missing "graphviz" "dot"  # Graphviz provides the 'dot' tool

echo ""
echo "[✓] All tools installed."
echo "[→] You can now build your project, generate docs with 'doxygen', or open the workspace in VS Code."
