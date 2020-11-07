#!/bin/bash

mkdir release
cp CoCOM.exe release
cd release

../../tools/mingw-bundledlls/mingw-bundledlls --copy CoCOM.exe
windeployqt CoCOM.exe
