#!/bin/bash

mkdir build
cd build

cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja -j $(nproc)

mkdir release
cp CoCOM.exe release
cd release

../../tools/mingw-bundledlls/mingw-bundledlls --copy CoCOM.exe
windeployqt CoCOM.exe
