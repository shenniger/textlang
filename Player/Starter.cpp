#include <iostream>
#include <fstream>
#include <Error.hpp>
#include <Archive.hpp>
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
  // reading assets
  std::ifstream f("test", std::ios::binary); // TODO

  TextAdventure ta;
  Archive ar(&f);
  ar &ta;

  f.close();

  te = TextEngine(ta);

  doAnswer(te.begin());

  for (;;) {
    std::string query;
    std::getline(std::cin, query);
    doAnswer(te.query(query));
  }
}
