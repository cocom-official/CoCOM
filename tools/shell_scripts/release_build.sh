cd $(pwd)/../$1
rm -rf *
cmake ../CoCOM -GNinja -DCMAKE_BUILD_TYPE=Release -DCPACK_GENERATOR=WIX ..
ninja -j $(nproc)
cpack

cmake ../CoCOM -GNinja -DCMAKE_BUILD_TYPE=Release -DCPACK_GENERATOR=7Z ..
ninja -j $(nproc)
cpack
