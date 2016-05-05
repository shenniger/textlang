#!/bin/bash
# BUILD SCRIPT
# multithreaded
# to be extended later

mkdir -p Build/cli

#serializationMode="-DUSE_BOOST_SERIALIZATION -lboost_serialization"
serializationMode="-DUSE_OWN_SERIALIZATION"

$@ -Iheader -std=c++14 -Wall -Wextra -pedantic cli/Compiler.cpp -o Build/TextLangCompiler.build -fvisibility=hidden -lboost_serialization -lboost_filesystem -lboost_system $serializationMode &
$@ -Iheader -std=c++14 -Wall -Wextra -pedantic cli/Player.cpp -o Build/TextLangPlayer.build -fvisibility=hidden -lboost_serialization -lboost_filesystem -lboost_system $serializationMode &

#emscripten
cp -r -u web/html Build
#em++ --pre-js web/jsPre.js -s EXPORTED_FUNCTIONS="['_loadTextAdventure', '_beginTextAdventure', '_queryTextAdventure', '_choiceBoxQueryTextAdventure', '_answerText', '_answerChoiceBoxIndex', '_answerChoiceBoxSize', '_answerChoiceBoxNum', '_answerChoiceBoxEntry', '_destroyAnswer', '_destroyTextAdventure']" -s ASYNCIFY=1 -O3 -o Build/html/jsbridge.js web/bridge.cpp -Iheader -std=c++14 -DUSE_OWN_SERIALIZATION

wait
