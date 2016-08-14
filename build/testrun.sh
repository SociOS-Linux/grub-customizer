#!/bin/bash
# compile, install and run this project for testing

set -e

currentDir=$(dirname `readlink -f $0`)

if ! [ -e "$currentDir/tmp" ] ; then
    mkdir "$currentDir/tmp";
fi

cd "$currentDir/tmp"

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX="$currentDir/tmp" -DPOLICYKIT_FILENAME=gc_test.policy "$currentDir/.."
make install