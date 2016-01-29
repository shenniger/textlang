#!/bin/bash
# BUILD SCRIPT
# multithreaded
# to be extended later

mkdir -p Build

for i in $(tree src -fi -P "*.cpp" --noreport)
do
	if [[ -f $i ]]
	then
		mkdir -p `dirname Build/$i`
		$@ -c -Isrc -std=c++14 -Wall -Wextra -pedantic $i -o Build/$i.obj -fno-rtti -fvisibility=hidden &
		files="$files Build/$i.obj"
	fi
done

wait
$@ $files -o Build/TextLangCompiler.build
