#!/bin/sh

# This script will remove the binary, cmake cache and make the project again
# Then it will execute testing script
rm -f test_tp_sistemas
rm -rf CMakeFiles
rm -f CMakeCache.txt
cmake .
cmake --build .
make
