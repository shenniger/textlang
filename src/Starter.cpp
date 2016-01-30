#ifndef UNITTEST

#include <stddef.h>
#include <iostream>
#include <fstream>
#include "Parser.hpp"
#include "CodeStream.hpp"

std::string readFile(const std::string name) {
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

void readAnotherFile(NormalWriter &r, const std::string t) {
  Parser<NormalWriter> p(readFile(t), r);
  p();
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "USAGE: SOURCE_FILE" << std::endl;
    exit(1);
  }

  Parser<NormalWriter>::parse(readFile(argv[1]));
  return 0;
}

#endif
