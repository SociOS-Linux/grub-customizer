#!/bin/bash
# must be in project directory
shopt -s globstar

cppFiles=$(echo src/**/*.cpp)

for cppFile in $cppFiles ; do
	mkdir -p build/lzzbuild/`dirname $cppFile`
	cat "$cppFile" | sed 's/\/\/LZZ: //' > "build/lzzbuild/$cppFile"
	/media/daten/lzz "build/lzzbuild/$cppFile"
done
