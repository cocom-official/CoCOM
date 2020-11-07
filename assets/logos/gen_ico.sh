#!/bin/bash

# icoutils is needed, on msys2 mingw-w64-x86_64-icoutils / mingw32/mingw-w64-i686-icoutils.
cd "${0%/*}"

# icotool -c -o CoCOM.ico CoCOM_16.png CoCOM_32.png CoCOM_48.png CoCOM_128.png CoCOM_256.png CoCOM_512.png 

icotool -c -o CoCOM.ico CoCOM_32.png CoCOM_48.png CoCOM_128.png CoCOM_256.png
