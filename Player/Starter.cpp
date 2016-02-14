#include <iostream>
#include <fstream>
#include <Error.hpp>
#include <Serialization.hpp>
#include "TextEngine.hpp"

TextEngine te;

void doAnswer(TextEngine::Answer ans);

void displayChoiceBox(const TextEngine::Answer &ans) {
  ID i = 1;
  for (const auto &e : ans.Choices) {
    std::cout << "[" << i << "] => " << e.first << std::endl;
    i++;
  }

  ID chosen;
  std::cin >> chosen;
  std::cin.get();
  doAnswer(
      te.choiceBoxQuery(ans.ChoiceBoxIndex, ans.Choices.at(chosen - 1).second));
}

void doAnswer(TextEngine::Answer ans) {
  std::cout << ans.Text << "\n";
  if (ans.Ends) {
    std::exit(0);
  }
  if (ans.ChoiceBoxIndex != -1) {
    displayChoiceBox(ans);
  }
}

int main() {
  TextAdventure ta;

  // reading assets
  std::ifstream f("test", std::ios::binary); // TODO
  Serializer::Read(ta, f);
  f.close();

  te = TextEngine(ta);

  doAnswer(te.begin());

  for (;;) {
    std::string query;
    std::getline(std::cin, query);
    // TODO: this REALLY shouldn't be hardcoded
    if (query == "exit") {
      return 0;
    } else if (query == "load") {
      std::ifstream f(
          "savegame",
          std::ios::binary); // TODO: this should'n be hardcoded, too!
      Serializer::Read(te.state(), f);
    } else if (query == "save") {
      std::ofstream f(
          "savegame",
          std::ios::binary); // TODO: this should'n be hardcoded, too!
      Serializer::Write(te.state(), f);
    } else {
      doAnswer(te.query(query));
    }
  }
}
