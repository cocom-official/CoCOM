cd $(pwd)/../$1
rm .ninja_deps
rm .ninja_log
rm -rf *
cmake ../CoCOM -GNinja -DCMAKE_BUILD_TYPE=Debug
cd ../CoCOM
ninja -C ../$1 -j16