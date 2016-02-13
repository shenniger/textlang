#include <iostream>
#include <fstream>
#include <Error.hpp>
#include <Archive.hpp>
#include "TextEngine.hpp"

int main() {
  // reading assets
  std::ifstream f("test", std::ios::binary); // TODO

  TextAdventure ta;

  Archive ar(&f);
  ar &ta;

  f.close();

  TextEngine te(ta);

  std::cout << te.begin().Text;

  for (;;) {
    std::string query;
    std::getline(std::cin, query);
    std::cout << te.query(query).Text << "\n";
  }
}
