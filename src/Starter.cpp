#ifndef UNITTEST

#include <iostream>
#include <fstream>
#include "Parser.hpp"
#include "CodeStream.hpp"

std::string readFile(std::string name) {
  std::ifstream stream(name, std::ios::in | std::ios::binary);
  stream.seekg(0, std::ios::end);
  std::string res;
  res.reserve(stream.tellg());
  stream.seekg(0, std::ios::beg);
  while (!stream.eof()) {
    res += stream.get();
  }
  stream.close();
  return res.substr(0, res.length() - 1);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "USAGE: SOURCE_FILE" << std::endl;
    exit(1);
  }

  Parser(readFile(argv[1]))();
  /*CodeStream a(readFile(argv[1]));
  while (!a.eof())
    std::cout << a.next();*/
  return 0;
}

#endif
