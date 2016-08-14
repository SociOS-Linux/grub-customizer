#!/bin/bash
# convert general project to c++ project with dependencies for grub customizer
set -e

currentDir=$(dirname `readlink -f $0`)
dataDir="$currentDir/data"
mainDir="$currentDir/../.."

if ! [ -e "$mainDir/.settings" ] ; then
	cp -rv "$dataDir/settings" "$mainDir/.settings"
else
	echo ".settings already installed"
fi

if ! [ -e "$mainDir/.cproject" ] ; then
	cp -v "$dataDir/cproject" "$mainDir/.cproject"
else
	echo ".cproject already installed"
fi

if ! grep 'org\.eclipse\.cdt\.core\.cnature' "$mainDir/.project" > /dev/null ; then
	patch "$mainDir/.project" "$dataDir/project.patch"
else
	echo ".project already patched"
fi

# manual way
#- create project as Makefile project -> Empty project
#- import the includes from build/eclipse_gtkmm-3.0_includes.xml
#- build command: build/testrun.sh
#- add -std=c++11 at projects settings: C/C++ General -> Preprocessor Include Paths, Macros -> Providers
#- build the project
#- choose the binary at build/tmp/grub-customizer at the run/debug settings

# to do on both ways:
#- rerun the indexer