# CoCOM

CoCOM is a Serial Port Utility for Embedded Software Engineer.

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

## Feature

- [ ] basic send && receive
  - [x] receive text
  - [x] send text
  - [ ] receive Hex
  - [ ] send Hex
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
- [ ] small but useful feature
  - [x] window pin
  - [ ] save text output to file
- [ ] lua embed
  - [ ] new data received callback
  - [ ] new text line received callback
  - [ ] send data through lua scripts
  - [ ] protocol support
    - [ ] protocol decode
    - [ ] protocol encode
