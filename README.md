# CoCOM

CoCOM is a Serial Port Utility for Embedded Software Engineer.

``` bash
# install dependence
pacman -S base-devel git
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw64/mingw-w64-x86_64-qt-creator
pacman -S mingw64/mingw-w64-x86_64-cmake
pacman -S mingw64/mingw-w64-x86_64-ninja

# build
mkdir build && cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ..
# or
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja -j $(nproc)

cpack -G WIX
cpack -G 7Z
```
