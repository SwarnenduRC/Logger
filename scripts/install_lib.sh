#!/usr/bin/env bash
set -e

LIB_DIR="lib"    # lib directory
HEADER_DIR="include"   # include directory

# Make installation directory variables
INSTALL_LIB_DIR="/usr/local/lib"
INSTALL_INCLUDE_DIR="/usr/local/include/logger"

echo "Installing library to $INSTALL_LIB_DIR"
echo "Installing headers to $INSTALL_INCLUDE_DIR"

# Create target directories if not exist
sudo mkdir -p "$INSTALL_LIB_DIR"
sudo mkdir -p "$INSTALL_INCLUDE_DIR"

# Copy library file recursively
sudo cp -r "$LIB_DIR"/* "$INSTALL_LIB_DIR"

# Copy header files recursively
sudo cp -r "$HEADER_DIR"/* "$INSTALL_INCLUDE_DIR"

# Detect OS type
OS_TYPE=$(uname)

# Correct the embedded lib path from relative to absolute path on MacOS only
if [[ "$OS_TYPE" == "Darwin" ]]; then
  sudo install_name_tool -id /usr/local/lib/liblogger.so /usr/local/lib/liblogger.so
fi

# Update linker cache on Linux (not mandatory but recommended)
if [[ "$OS_TYPE" == "Linux" ]]; then
  sudo ldconfig
fi

echo "Installation completed successfully."

exit 0

