#!/usr/bin/env bash
set -e

BUILD_DIR="build"
CONFIG="Debug"

cmake -S . -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" --config "$CONFIG"

EXE=$(find "$BUILD_DIR/$CONFIG" "$BUILD_DIR" \
  -type f -executable \
  ! -name "*.dll" \
  ! -name "*.so" |
  head -n 1)

if [ -z "$EXE" ]; then
  echo "No executable found."
  exit 1
fi

echo "Running: $EXE"
exec "$EXE"
