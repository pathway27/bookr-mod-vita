#!/bin/sh

rm -rf Release
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
