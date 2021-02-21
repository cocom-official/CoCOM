# CoCOM

CoCOM is a Serial Port Utility for Embedded Software Engineer.

## Feature

- [ ] basic send && receive
  - [x] receive text
  - [x] send text
  - [x] receive Hex
  - [x] send Hex
  - [ ] send command
- [ ] high level send && receive
  - [x] text encoding
  - [x] line break
  - [ ] auto send
- [ ] delectable output view
  - [x] ligature support
  - [x] success && warn && error colorful hint
  - [ ] output filer
  - [ ] custom colorful hint
  - [ ] Ctrl + F, find anything in output view
- [ ] high DPI support
  - [x] auto scaling according to dpi
  - [x] auto scaling when moves betweens different dpi screen
- [ ] save text output to file
- [ ] lua embed
  - [ ] new data received callback
  - [ ] new text line received callback
  - [ ] send data through lua scripts
  - [ ] protocol support
    - [ ] protocol decode
    - [ ] protocol encode
- [ ] small but useful feature
  - [x] window pin

## Build

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
