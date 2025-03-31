#!/bin/bash

cmake -B build_64 -DCMAKE_BUILD_TYPE=Debug
cmake -B build_64rel -DCMAKE_BUILD_TYPE=Release
cmake -B build_64RelWithDebInfo -DCMAKE_BUILD_TYPE=RelWithDebInfo

echo "Build directories created successfully!"