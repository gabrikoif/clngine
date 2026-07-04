#!/usr/bin/env bash
set -e

BUILD_DIR="build"

# Configure and Build
echo "Configuring with CMake..."
cmake -S . -B "$BUILD_DIR"

echo "Building project..."
cmake --build "$BUILD_DIR"

# Find the executable
# Searches in the build directory root (no subfolder like Debug)
EXE=$(find "$BUILD_DIR" \
  -maxdepth 1 \
  -type f \
  -executable \
  ! -name "*.dll" \
  ! -name "*.so" \
  ! -name "*.dylib" \
  | head -n 1)

if [ -z "$EXE" ]; then
  echo "Error: No executable found in $BUILD_DIR."
  exit 1
fi

echo "Running: $EXE"
exec "$EXE"