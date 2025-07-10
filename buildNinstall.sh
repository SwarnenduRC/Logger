#!/bin/bash
set -e  # Exit immediately if a command exits with a non-zero status

# Default values
BUILD_TYPE="release"
BUILD_TEST="no"
LIB_TYPE="static"
FILE_LOGGING="no"
FILE_SIZE="10MB"
LOG_FILE_PATH=""
LOG_FILE_NAME=""
LOG_FILE_EXTN=""

print_global_help() {
  cat <<EOF
Usage: $0 [OPTIONS]

Optional arguments (case insensitive):

  -BUILD_TYPE=<release|debug>    (default: release)
      Set the build type.

  -BUILD_TEST=<yes|no>           (default: no)
      Whether to build tests.

  -LIB_TYPE=<static|shared>      (default: static)
      Type of library to build.

  -FILE_LOGGING=<yes|no>         (default: no)
      Enable logging to a file (yes) or console (no).

  -FILE_SIZE=<size>              (default: 10MB)
      Maximum file size for logging.
      Valid sizes: integer with optional suffix K, M, G (e.g. 4KB, 10MB, 1G).
      Minimum allowed size: 4KB.

  -LOG_FILE_PATH=<path>           (optional)
      Export LOG_FILE_PATH if path exists.

  -LOG_FILE_NAME=<name>           (optional)
      Export LOG_FILE_NAME.

  -LOG_FILE_EXTN=<extension>      (optional)
      Export LOG_FILE_EXTN.

Help options:

  --help, -h                     Show this help message.

  -<ARG_NAME> --help, -h         Show possible values and description for ARG_NAME.

Examples:

  $0 -BUILD_TYPE=debug -BUILD_TEST=yes -FILE_LOGGING=yes -FILE_SIZE=20MB
  $0 --help
  $0 -FILE_LOGGING --help
EOF
}

print_arg_help() {
  local arg="$1"
  case "$arg" in
    BUILD_TYPE)
      cat <<EOF
-BUILD_TYPE possible values (case insensitive):

  release   - Build the release version (default).
  debug     - Build the debug version.
EOF
      ;;
    BUILD_TEST)
      cat <<EOF
-BUILD_TEST possible values (case insensitive):

  yes       - Build the tests.
  no        - Do not build the tests (default).
EOF
      ;;
    LIB_TYPE)
      cat <<EOF
-LIB_TYPE possible values (case insensitive):

  static    - Build static libraries (default).
  shared    - Build shared libraries.
EOF
      ;;
    FILE_LOGGING)
      cat <<EOF
-FILE_LOGGING possible values (case insensitive):

  yes       - Enable logging to a file.
  no        - Log output to console (default).
EOF
      ;;
    FILE_SIZE)
      cat <<EOF
-FILE_SIZE:

  Maximum file size for logging.

  Format: integer optionally followed by K, M, or G suffix (case insensitive).
  Examples: 4096, 4K, 10MB, 1G.

  Minimum allowed size: 4KB.
EOF
      ;;
    LOG_FILE_PATH)
      cat <<EOF
-LOG_FILE_PATH:

  Optional path to export as LOG_FILE_PATH.
  If provided, the path must exist on the system.
EOF
      ;;
    LOG_FILE_NAME)
      cat <<EOF
-LOG_FILE_NAME:

  Optional file name to export as LOG_FILE_NAME.
EOF
      ;;
    LOG_FILE_EXTN)
      cat <<EOF
-LOG_FILE_EXTN:

  Optional file extension to export as LOG_FILE_EXTN.
EOF
      ;;
    *)
      echo "No help available for argument '$arg'."
      ;;
  esac
}

# Function to convert size string to bytes for validation
# Supports optional suffixes K, M, G (case insensitive)
size_to_bytes() {
  local size_str="$1"
  local num_part
  local unit_part
  local num_bytes

  # Extract numeric part and unit part
  if [[ "$size_str" =~ ^([0-9]+)([KkMmGg]?[Bb]?)?$ ]]; then
    num_part="${BASH_REMATCH[1]}"
    unit_part="${BASH_REMATCH[2]}"
  else
    echo "Error: Invalid FILE_SIZE format: $size_str"
    exit 1
  fi

  # Normalize unit
  unit_part=$(echo "$unit_part" | tr '[:upper:]' '[:lower:]')

  case "$unit_part" in
    ""|"b")
      num_bytes=$num_part
      ;;
    "k"|"kb")
      num_bytes=$((num_part * 1024))
      ;;
    "m"|"mb")
      num_bytes=$((num_part * 1024 * 1024))
      ;;
    "g"|"gb")
      num_bytes=$((num_part * 1024 * 1024 * 1024))
      ;;
    *)
      echo "Error: Unknown unit in FILE_SIZE: $unit_part"
      exit 1
      ;;
  esac

  echo "$num_bytes"
}

# Parse arguments
if [[ $# -eq 0 ]]; then
  # No arguments, just proceed with defaults
  :
else
  # Check if first argument is global help
  if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    print_global_help
    exit 0
  fi

  # Parse arguments one by one
  while [[ $# -gt 0 ]]; do
    arg="$1"

    # Check if argument is help for a specific option
    if [[ "$arg" =~ ^-([A-Za-z_]+)$ ]] && ([[ "$2" == "--help" ]] || [[ "$2" == "-h" ]]); then
      # e.g. -BUILD_TYPE --help
      arg_name="${BASH_REMATCH[1]}"
      print_arg_help "$arg_name"
      exit 0
    fi

    # Check if argument itself is a help flag
    if [[ "$arg" == "--help" || "$arg" == "-h" ]]; then
      print_global_help
      exit 0
    fi

    # Parse key=value arguments
    if [[ "$arg" =~ ^-([A-Za-z_]+)=(.*)$ ]]; then
      key="${BASH_REMATCH[1]}"
      value="${BASH_REMATCH[2]}"
      key_upper=$(echo "$key" | tr '[:lower:]' '[:upper:]')
      value_lower=$(echo "$value" | tr '[:upper:]' '[:lower:]')

      case "$key_upper" in
        BUILD_TYPE)
          if [[ "$value_lower" == "release" || "$value_lower" == "debug" ]]; then
            BUILD_TYPE="$value_lower"
          else
            echo "Error: Invalid value for BUILD_TYPE: $value"
            echo "Use -BUILD_TYPE --help for valid options."
            exit 1
          fi
          ;;
        BUILD_TEST)
          if [[ "$value_lower" == "yes" || "$value_lower" == "no" ]]; then
            BUILD_TEST="$value_lower"
          else
            echo "Error: Invalid value for BUILD_TEST: $value"
            echo "Use -BUILD_TEST --help for valid options."
            exit 1
          fi
          ;;
        LIB_TYPE)
          if [[ "$value_lower" == "static" || "$value_lower" == "shared" ]]; then
            LIB_TYPE="$value_lower"
          else
            echo "Error: Invalid value for LIB_TYPE: $value"
            echo "Use -LIB_TYPE --help for valid options."
            exit 1
          fi
          ;;
        FILE_LOGGING)
          if [[ "$value_lower" == "yes" || "$value_lower" == "no" ]]; then
            FILE_LOGGING="$value_lower"
          else
            echo "Error: Invalid value for FILE_LOGGING: $value"
            echo "Use -FILE_LOGGING --help for valid options."
            exit 1
          fi
          ;;
        FILE_SIZE)
          # Validate FILE_SIZE value
          bytes=$(size_to_bytes "$value")
          min_bytes=$((4 * 1024))  # 4KB minimum
          if (( bytes < min_bytes )); then
            echo "Error: FILE_SIZE must be at least 4KB."
            exit 1
          fi
          FILE_SIZE="$value"
          ;;
        LOG_FILE_PATH)
          # Validate path exists if not empty
          if [[ -n "$value" ]]; then
            if [[ ! -d "$value" ]]; then
              echo "Error: LOG_FILE_PATH '$value' does not exist or is not a directory."
              exit 1
            fi
          fi
          LOG_FILE_PATH="$value"
          ;;
        LOG_FILE_NAME)
          LOG_FILE_NAME="$value"
          ;;
        LOG_FILE_EXTN)
          LOG_FILE_EXTN="$value"
          ;;
        *)
          echo "Warning: Unknown argument '$key'. Ignored."
          ;;
      esac
    else
      echo "Error: Invalid argument format: $arg"
      echo "Use --help for usage."
      exit 1
    fi

    shift
  done
fi

# Export LOG_FILE_* only if FILE_LOGGING=yes and variables are non-empty
if [[ "$FILE_LOGGING" == "yes" ]]; then
    export FILE_LOGGING
    export FILE_SIZE
  if [[ -n "$LOG_FILE_PATH" ]]; then
    export LOG_FILE_PATH
  fi
  if [[ -n "$LOG_FILE_NAME" ]]; then
    export LOG_FILE_NAME
  fi
  if [[ -n "$LOG_FILE_EXTN" ]]; then
    export LOG_FILE_EXTN
  fi
fi

# Print the final values (for demonstration)
echo "BUILD_TYPE=$BUILD_TYPE"
echo "BUILD_TEST=$BUILD_TEST"
echo "LIB_TYPE=$LIB_TYPE"
echo "FILE_LOGGING=$FILE_LOGGING"
echo "FILE_SIZE=$FILE_SIZE"
echo "LOG_FILE_PATH=${LOG_FILE_PATH:-<not set>}"
echo "LOG_FILE_NAME=${LOG_FILE_NAME:-<not set>}"
echo "LOG_FILE_EXTN=${LOG_FILE_EXTN:-<not set>}"

echo ""
echo ""

# Generate the enviornment header file now
python3 ./scripts/generate_env_vars_header.py
status=$?
if [[ $status -ne 0 ]]; then
  echo "Error: Failed to generate environment header file (exit code $status)"
  exit $status
fi

echo ""

# Script to install google test if testing is required
if [[ "$BUILD_TEST" == "yes" ]]; then
    ./scripts/install_gtest.sh
    status_gtest=$?
    if [[ $status_gtest -ne 0 ]]; then
        echo "Error: Google Test installation script failed with exit code $status_gtest"
        exit $status_gtest
    else
        echo "Google Test installation completed successfully"
    fi
fi

# Script to install fmt library
./scripts/install_fmt.sh
status_fmt=$?
if [[ $status_fmt -ne 0 ]]; then
    echo "Error: fmt library installation script failed with exit code $status_fmt"
    exit $status_fmt
else
    echo "fmt library installation completed successfully"
fi

echo ""

# Now do a clean build
make clean; make BUILD_TYPE="$BUILD_TYPE" LIB_TYPE="$LIB_TYPE" BUILD_TEST="$BUILD_TEST"

# Install the generated library to the system
./scripts/install_lib.sh
status_install_lib=$?
if [[ $status_install_lib -ne 0 ]]; then
  echo "Error: logger library installation script failed with exit code $status_install_lib"
  echo "Cleaning solution"
  make clean
  exit $status_install_lib
fi

exit 0
