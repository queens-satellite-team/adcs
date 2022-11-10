#!/bin/bash

#### clean ####
makefile=./Makefile
cmake_cache=./CMakeCache.txt
cmake_install=./cmake_install.cmake
misc_dir=./CMakeFiles
bin_dir=./bin

if test -f $makefile; then
    echo "removing $makefile"
    rm $makefile
fi
if test -f $cmake_cache; then
    echo "removing $cmake_cache"
    rm $cmake_cache
fi
if test -f $cmake_install; then
    echo "removing $cmake_install"
    rm $cmake_install
fi
if test -d $misc_dir; then
    echo "removing $misc_dir"
    rm -r $misc_dir
fi
if test -d $bin_dir; then
    echo "removing $bin_dir"
    rm -r $bin_dir
fi

#### build ####

if [[ "$1" != "clean" ]]
then
    cmake . -Wno-dev
fi
