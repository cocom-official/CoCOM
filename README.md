# CoCOM

CoCOM is a Serial Port Utility for Embedded Software Engineer.

## Feature

- [x] basic send && receive
  - [x] receive text
  - [x] send text
  - [x] receive Hex
  - [x] send Hex
  - [x] send command
- [x] high level send && receive
  - [x] text encoding
  - [x] line break
  - [x] auto periodic send
- [ ] delectable output view
  - [x] ligature support
  - [x] success && warn && error colorful hint
  - [ ] output filer
  - [ ] custom colorful hint
  - [x] Ctrl + F, find anything in output view
  - [ ] virtual terminal sequences support
- [x] high DPI support
  - [x] auto scaling according to dpi
  - [x] auto scaling when moves betweens different dpi screen
- [x] save text output to file with encoding support
- [ ] get data from output
  - [x] output to data
  - [ ] export data to file(.csv)
  - [x] initial support plot data
  - [ ] full support plot data
- [ ] lua embed
  - [x] lua console
  - [x] lua with CoCOM base extension
  - [x] read data/text/line through lua scripts
  - [x] send data/text through lua scripts
  - [ ] new data received callback
  - [ ] protocol support
    - [ ] protocol decode
    - [ ] protocol encode
  - [ ] send && receive file
  - [ ] upgrade MCU firmware
- [ ] USB bulk support
  - [ ] protocol support
  - [ ] upgrade MCU firmware
- [ ] ADB support
  - [ ] ADB log support
  - [ ] ADB shell support
- [ ] small but useful feature
  - [x] window pin
  - [x] global shortcuts
  - [ ] more...
- [ ] to bo continue

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
