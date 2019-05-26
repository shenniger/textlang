#!/bin/bash
# BUILD SCRIPT
# multithreaded
# to be extended later

mkdir -p Build/cli

#serializationMode="-DUSE_BOOST_SERIALIZATION -lboost_serialization"
serializationMode="-DUSE_OWN_SERIALIZATION"

$@ -Iheader -std=c++14 -Wall -Wextra -pedantic cli/Compiler.cpp -o Build/TextLangCompiler.build -fvisibility=hidden -lboost_serialization -lboost_filesystem -lboost_system $serializationMode &
$@ -Iheader -std=c++14 -Wall -Wextra -pedantic cli/Player.cpp -o Build/TextLangPlayer.build -fvisibility=hidden -lboost_serialization -lboost_filesystem -lboost_system $serializationMode &

wait
