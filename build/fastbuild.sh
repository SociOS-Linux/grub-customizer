#!/bin/bash
set -e
shopt -s globstar

if [[ $# != 1 ]] ; then
	echo "I need the root path of the project as argument #1"
	exit 1;
fi

cd "$1";

if ! [[ -e build/prepare-fastbuild ]] || [[ build/prepare-fastbuild.cpp -nt build/prepare-fastbuild ]] ; then
	g++ build/prepare-fastbuild.cpp -std=c++11 -o build/prepare-fastbuild
fi

./build/prepare-fastbuild . src/**/*.hpp

if ! [[ -e build/dump-autoload ]] || [[ build/dump-autoload.cpp -nt build/dump-autoload ]] ; then
	g++ build/dump-autoload.cpp -std=c++11 -o build/dump-autoload
fi

cd "build/fastbuild";

../dump-autoload . src/**/*.cpp
