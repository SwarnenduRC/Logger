#!/usr/bin/env bash
set -e

echo "Checking for Google Test (gtest)..."

# Function to check if gtest library is installed
check_gtest() {
  # Try pkg-config first
  if pkg-config --exists gtest; then
    return 0
  fi

  # Check common library locations for libgtest.a or libgtest.so/dylib
  if ldconfig -p 2>/dev/null | grep -q libgtest; then
    return 0
  fi

  # macOS: check if brew package installed
  if command -v brew >/dev/null 2>&1; then
    if brew list googletest >/dev/null 2>&1; then
      return 0
    fi
  fi

  return 1
}

# Function to install gtest via package manager or build from source
install_gtest() {
  OS_TYPE=$(uname)
  echo "Installing Google Test on $OS_TYPE..."

  if [[ "$OS_TYPE" == "Darwin" ]]; then
    # macOS install
    if ! command -v brew >/dev/null 2>&1; then
      echo "Homebrew not found. Please install Homebrew first: https://brew.sh/"
      exit 1
    fi
    echo "Installing googletest via Homebrew..."
    brew install googletest
    echo "Note: Homebrew does not symlink googletest headers/libs automatically."
    echo "You may need to add the following to your CMake or build scripts:"
    echo "  -DCMAKE_PREFIX_PATH=$(brew --prefix googletest)"
  elif [[ "$OS_TYPE" == "Linux" ]]; then
    # Linux install
    if command -v apt-get >/dev/null 2>&1; then
      echo "Installing libgtest-dev via apt..."
      sudo apt-get update
      sudo apt-get install -y libgtest-dev cmake build-essential

      # Build and install manually because apt only installs source
      GTEST_SRC_DIR="/usr/src/gtest"
      if [[ -d "$GTEST_SRC_DIR" ]]; then
        cd "$GTEST_SRC_DIR"
        sudo cmake .
        sudo make
        sudo cp *.a /usr/lib || sudo cp *.a /usr/lib/x86_64-linux-gnu/
        echo "Google Test installed."
      else
        echo "Google Test source directory not found at $GTEST_SRC_DIR"
        exit 1
      fi
    else
      # Fallback: build from source
      echo "apt-get not found. Building Google Test from source..."
      git clone https://github.com/google/googletest.git
      cd googletest
      mkdir -p build && cd build
      cmake .. -DCMAKE_BUILD_TYPE=Release
      make
      sudo make install
      cd ../..
      rm -rf googletest
    fi
  else
    echo "Unsupported OS: $OS_TYPE"
    exit 1
  fi
}

if check_gtest; then
  echo "Google Test is already installed."
else
  echo "Google Test not found. Installing..."
  install_gtest
fi

exit 0