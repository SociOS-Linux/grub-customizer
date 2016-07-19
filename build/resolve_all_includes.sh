#!/bin/bash
currentDir=$(dirname `readlink -f $0`)

function resolve(){
	local file="$1"
	local extension="$2"
	local prefix="$3";
	echo "resolving $file.$extension"
	#echo "#include \"${prefix}lib/Helper.hpp\"" > "$currentDir/../src/$file.inc.$extension";
	"$currentDir/resolve_includes.sh" "$file.$extension" "$prefix" > "$currentDir/../src/$file.inc.$extension";
}

resolve "Bootstrap/GtkApplication" "cpp" ../
resolve "Bootstrap/FactoryImpl/GlibThread" "cpp" ../../
resolve "Bootstrap/FactoryImpl/GLibRegex" "cpp" ../../
resolve "Bootstrap/FactoryImpl/GlibThread" "cpp" ../../
resolve "Bootstrap/GtkView" "cpp" ../
resolve "main/proxy" "cpp" ../
resolve "main/client" "cpp" ../