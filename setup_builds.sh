#!/bin/bash

# Check for --clean argument
if [[ "$1" == "--clean" ]]; then
  echo "Cleaning build directories..."

  # Check if the build directories exist before attempting to remove them
  if [ -d "build_64" ]; then
    rm -rf build_64
    echo "Removed build_64 directory."
  fi

  if [ -d "build_64rel" ]; then
    rm -rf build_64rel
    echo "Removed build_64rel directory."
  fi

  if [ -d "build_64RelWithDebInfo" ]; then
    rm -rf build_64RelWithDebInfo
    echo "Removed build_64RelWithDebInfo directory."
  fi

  echo "Build directories cleaned!"
fi

# Create build directories
cmake -B build_64 -DCMAKE_BUILD_TYPE=Debug
cmake -B build_64rel -DCMAKE_BUILD_TYPE=Release
cmake -B build_64RelWithDebInfo -DCMAKE_BUILD_TYPE=RelWithDebInfo

echo "Build directories created successfully!"
