#ifndef UNITTEST

#include <stddef.h>
#include <iostream>
#include <fstream>
#include "Parser.hpp"
#include "CodeStream.hpp"

std::string readFile(std::string name) {
  std::ifstream stream(name, std::ios::in | std::ios::binary);
  stream.seekg(0, std::ios::end);
  std::string res;
  size_t len = stream.tellg();
  res.resize(len);
  stream.seekg(0, std::ios::beg);
  stream.read(&res[0], len);
  stream.close();
  return res;
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
