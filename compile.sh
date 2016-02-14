#!/bin/bash
# BUILD SCRIPT
# multithreaded
# to be extended later

mkdir -p Build

for i in $(tree Compiler -fi -P "*.cpp" --noreport)
do
	if [[ -f $i ]]
	then
		mkdir -p `dirname Build/$i`
		$@ -c -IGeneral -std=c++14 -Wall -Wextra -pedantic $i -o Build/$i.obj -fvisibility=hidden &
		files="$files Build/$i.obj"
	fi
done

wait
$@ $files -o Build/TextLangCompiler.build -lboost_locale -lboost_serialization

for i in $(tree Player -fi -P "*.cpp" --noreport)
do
	if [[ -f $i ]]
	then
		mkdir -p `dirname Build/$i`
		$@ -c -IGeneral -std=c++14 -Wall -Wextra -pedantic $i  -o Build/$i.obj -fvisibility=hidden &
		files2="$files2 Build/$i.obj"
	fi
done

wait
$@ $files2 -o Build/TextLangPlayer.build -lboost_serialization
