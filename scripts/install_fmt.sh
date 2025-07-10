#!/usr/bin/env bash
set -e

echo "Checking for fmt library..."

# Function to check if fmt is installed (using pkg-config or find library)
check_fmt() {
  if pkg-config --exists fmt; then
    return 0
  fi

  # Check common lib locations for libfmt.a or libfmt.so/dylib
  if ldconfig -p 2>/dev/null | grep -q libfmt; then
    return 0
  fi

  # macOS: check brew
  if command -v brew >/dev/null 2>&1; then
    if brew list fmt >/dev/null 2>&1; then
      return 0
    fi
  fi

  return 1
}

install_fmt() {
  OS_TYPE=$(uname)
  echo "Installing fmt library on $OS_TYPE..."

  if [[ "$OS_TYPE" == "Darwin" ]]; then
    if ! command -v brew >/dev/null 2>&1; then
      echo "Homebrew not found. Please install Homebrew first: https://brew.sh/"
      exit 1
    fi
    echo "Installing fmt via Homebrew..."
    brew install fmt
  elif [[ "$OS_TYPE" == "Linux" ]]; then
    if command -v apt-get >/dev/null 2>&1; then
      echo "Installing libfmt-dev via apt..."
      sudo apt-get update
      sudo apt-get install -y libfmt-dev
    else
      echo "apt-get not found. Building fmt from source..."
      git clone https://github.com/fmtlib/fmt.git
      cd fmt
      mkdir -p build && cd build
      cmake .. -DCMAKE_BUILD_TYPE=Release
      make
      sudo make install
      cd ../..
      rm -rf fmt
    fi
  else
    echo "Unsupported OS: $OS_TYPE"
    exit 1
  fi
}

if check_fmt; then
  echo "fmt library is already installed."
else
  echo "fmt library not found. Installing..."
  install_fmt
fi

exit 0