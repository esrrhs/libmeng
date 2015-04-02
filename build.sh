#! /bin/sh

BUILD_FLAG=""

if [ $# == 1 ] && [ $1 == "release" ];then
    BUILD_FLAG=" -DREMOD=ON"
fi

#lib
rm CMakeCache.txt -rf
rm CMakeFiles -rf
rm cmake_install.cmake -rf
rm Makefile -rf
cmake . $BUILD_FLAG
make clean
make -j5

#test
cd test
rm CMakeCache.txt -rf
rm CMakeFiles -rf
rm cmake_install.cmake -rf
rm Makefile -rf
cmake . $BUILD_FLAG
make clean
make -j5
cd ..

echo "build ok"
