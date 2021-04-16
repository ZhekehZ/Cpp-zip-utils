#!/bin/bash

rm -rf build
mkdir build

pushd build || exit 1
conan install ..
cmake ..
make
./test/zip_utils_all_tests
popd || exit 1