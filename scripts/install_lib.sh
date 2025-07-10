#!/usr/bin/env bash
set -e

LIB_DIR="lib"    # lib directory
HEADER_DIR="include"   # include directory

# Detect OS type
OS_TYPE=$(uname)

# Detect if brew is installed and get prefix
if command -v brew >/dev/null 2>&1; then
  BREW_PREFIX=$(brew --prefix)
else
  BREW_PREFIX=""
fi

if [[ "$BREW_PREFIX" != "" ]]; then
  # Use Homebrew prefix on macOS or Linux
  INSTALL_LIB_DIR="$BREW_PREFIX/lib"
  INSTALL_INCLUDE_DIR="$BREW_PREFIX/include"
else
  # No brew detected - fallback defaults
  if [[ "$OS_TYPE" == "Darwin" ]]; then
    arch=$(uname -m)
    if [[ "$arch" == "arm64" ]]; then
      # Apple Silicon macOS default Homebrew prefix
      INSTALL_LIB_DIR="/opt/homebrew/lib"
      INSTALL_INCLUDE_DIR="/opt/homebrew/include"
    else
      # Intel macOS default prefix
      INSTALL_LIB_DIR="/usr/local/lib"
      INSTALL_INCLUDE_DIR="/usr/local/include"
    fi
  elif [[ "$OS_TYPE" == "Linux" ]]; then
    # Linux fallback prefix
    INSTALL_LIB_DIR="/usr/local/lib"
    INSTALL_INCLUDE_DIR="/usr/local/include"
  else
    echo "Unsupported OS: $OS_TYPE"
    exit 1
  fi
fi

echo "Installing library to $INSTALL_LIB_DIR"
echo "Installing headers to $INSTALL_INCLUDE_DIR"

# Create target directories if not exist
sudo mkdir -p "$INSTALL_LIB_DIR"
sudo mkdir -p "$INSTALL_INCLUDE_DIR"

# Copy library file recursively
sudo cp -r "$LIB_DIR"/* "$INSTALL_LIB_DIR"

# Copy header files recursively
sudo cp -r "$HEADER_DIR"/* "$INSTALL_INCLUDE_DIR"

# Update linker cache on Linux
if [[ "$OS_TYPE" == "Linux" ]]; then
  sudo ldconfig
fi

echo "Installation completed successfully."

exit 0