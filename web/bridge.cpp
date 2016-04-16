#include <emscripten.h>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <textlang/Player/TextEngine.hpp>
#include <textlang/Serialization.hpp>

using AdvHandle = intptr_t;
using AdvPointer = TextEngine *;
static_assert(sizeof(AdvHandle) == sizeof(AdvPointer), "Internal error!");

using AnsHandle = intptr_t;
using AnsPointer = TextEngine::Answer *;
static_assert(sizeof(AnsHandle) == sizeof(AnsPointer), "Internal error!");

using JSString = const char *;

TextAdventure loadTA(const JSString url) {
  TextAdventure res;
  const auto advPath = "/advsrc";
  emscripten_wget(url, advPath);
  std::ifstream f(advPath);
  if (!f) {
    EM_ASM(alert("ERROR: Couldn't load file!"););
  }
  Serializer::Read(res, f);
  EM_ASM(FS.unlink("/advsrc"); // can't use advPath here, we're in JS code!
         );
  EM_ASM_({
            console.log("Successfully loaded " + $0 + " nouns, " + $1 +
                        " verbs and " + $2 + " actions.");
          },
          res.Nouns.size(), res.Verbs.size(), res.Actions.size());
  return res;
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
void loadTextAdventure(JSString file) {
  auto adv = new TextEngine(loadTA(file));
  EM_ASM_({
	   __ta_last_func($0);
          }, reinterpret_cast<AdvHandle>(adv));
}
EMSCRIPTEN_KEEPALIVE
AnsHandle beginTextAdventure(const AdvHandle self_handle) {
  auto &self = *reinterpret_cast<AdvPointer>(self_handle);
  auto ans = new TextEngine::Answer(self.begin());
  return reinterpret_cast<AnsHandle>(ans);
}
EMSCRIPTEN_KEEPALIVE
AnsHandle queryTextAdventure(const AdvHandle self_handle, JSString query) {
  auto &self = *reinterpret_cast<AdvPointer>(self_handle);
  auto ans = new TextEngine::Answer(self.query(std::string(query)));
  return reinterpret_cast<AnsHandle>(ans);
}
EMSCRIPTEN_KEEPALIVE
AnsHandle choiceBoxQueryTextAdventure(const AdvHandle self_handle, const ID ChoiceBoxIndex, const ID ChoiceID) {
  auto &self = *reinterpret_cast<AdvPointer>(self_handle);
  auto ans = new TextEngine::Answer(self.choiceBoxQuery(ChoiceBoxIndex, ChoiceID));
  return reinterpret_cast<AnsHandle>(ans);
}
EMSCRIPTEN_KEEPALIVE
JSString answerText(const AnsHandle self_handle) {
  return reinterpret_cast<AnsPointer>(self_handle)->Text.c_str();
}
EMSCRIPTEN_KEEPALIVE
ID answerChoiceBoxIndex(const AnsHandle self_handle) {
  return reinterpret_cast<AnsPointer>(self_handle)->ChoiceBoxIndex;
}
EMSCRIPTEN_KEEPALIVE
size_t answerChoiceBoxSize(const AnsHandle self_handle) {
  return reinterpret_cast<AnsPointer>(self_handle)->Choices.size();
}
EMSCRIPTEN_KEEPALIVE
ID answerChoiceBoxNum(const AnsHandle self_handle, const size_t num) {
  return reinterpret_cast<AnsPointer>(self_handle)->Choices.at(num).second;
}
EMSCRIPTEN_KEEPALIVE
JSString answerChoiceBoxEntry(const AnsHandle self_handle, const size_t num) {
  return reinterpret_cast<AnsPointer>(self_handle)->Choices.at(num).first.c_str();
}
EMSCRIPTEN_KEEPALIVE
void destroyAnswer(const AnsHandle self_handle) {
  auto &self = *reinterpret_cast<AnsPointer>(self_handle);
  delete &self;
}
EMSCRIPTEN_KEEPALIVE
void destroyTextAdventure(const AdvHandle self_handle) {
  auto &self = *reinterpret_cast<AdvPointer>(self_handle);
  delete &self;
}
}
