#include <emscripten.h>
#include <emscripten/bind.h>
#include <fstream>
#include <sstream>
#include <string>
#include <textlang/Player/TextEngine.hpp>
#include <textlang/Serialization.hpp>

class TextAdventurePlayer {
private:
  TextEngine _engine;

  TextAdventure loadTA(std::string url) {
    TextAdventure res;
    const auto advPath = "/advsrc";
    emscripten_wget(url.c_str(), advPath);
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

public:
  TextAdventurePlayer(std::string url) : _engine(loadTA(url)) {}

  int getNum() { return 10; }

  TextEngine::Answer begin() { return _engine.begin(); }

  TextEngine::Answer query(std::string text) { return _engine.query(text); }

  TextEngine::Answer choiceBoxQuery(const ID ChoiceBox, const ID Option) {
    return _engine.choiceBoxQuery(ChoiceBox, Option);
  }

  void load(std::string s) {
    std::istringstream stream(s);
    Serializer::Read(_engine.state(), stream);
  }

  std::string save() {
    std::ostringstream s;
    Serializer::Write(_engine.state(), s);
    return s.str();
  }
};

EMSCRIPTEN_BINDINGS(AdventurePlayer) {
  emscripten::class_<TextAdventurePlayer>("Player")
      .constructor<std::string>()
      .function("begin", &TextAdventurePlayer::begin)
      .function("query", &TextAdventurePlayer::query)
      .function("choiceBoxQuery", &TextAdventurePlayer::choiceBoxQuery)
      .function("load", &TextAdventurePlayer::load)
      .function("save", &TextAdventurePlayer::save);

  emscripten::value_object<TextEngine::Answer>("Answer")
      .field("Text", &TextEngine::Answer::Text)
      .field("Ends", &TextEngine::Answer::Ends)
      .field("ChoiceBoxIndex", &TextEngine::Answer::ChoiceBoxIndex)
      .field("Choices", &TextEngine::Answer::Choices);

  emscripten::register_vector<std::pair<std::string, ID>>("VectorChoices");
  emscripten::value_object<std::pair<std::string, ID>>("PairChoice")
      .field("first", &std::pair<std::string, ID>::first)
      .field("second", &std::pair<std::string, ID>::second);
}
