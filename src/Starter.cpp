#ifndef UNITTEST

#include <stddef.h>
#include <iostream>
#include <fstream>
#include "Archive.hpp"
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
  if (argc != 3) {
    std::cout << "USAGE: SOURCE_FILE TARGET_FILE" << std::endl;
    exit(1);
  }

  auto a = Parser<NormalWriter>::parse(readFile(argv[1]))();
  std::ofstream f(argv[2], std::ios::binary);
  Archive ar(&f);
  ar &a;
  f.close();

  return 0;
}

#endif
