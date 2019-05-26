#!/bin/bash
# BUILD SCRIPT
# multithreaded
# to be extended later

mkdir -p Build
cp -r -u web/html Build
em++ --pre-js web/jsPre.js -s EXPORTED_FUNCTIONS="['_loadTextAdventure', '_beginTextAdventure', '_queryTextAdventure', '_choiceBoxQueryTextAdventure', '_answerText', '_answerChoiceBoxIndex', '_answerChoiceBoxSize', '_answerChoiceBoxNum', '_answerChoiceBoxEntry', '_destroyAnswer', '_destroyTextAdventure', '_malloc']" -s EXTRA_EXPORTED_RUNTIME_METHODS="['UTF8ToString', 'allocate', 'ALLOC_NORMAL']" -s ASYNCIFY=1 -o Build/html/jsbridge.js web/bridge.cpp -Iheader -std=c++14 -DUSE_OWN_SERIALIZATION -Os
