#ifndef UNITTEST

#include <stddef.h>
#include <iostream>
#include <fstream>
#include <textlang/Serialization.hpp>
#include <textlang/Compiler/Parser.hpp>

std::string curFile;

std::string readFile(const std::string name) {
  std::ifstream stream(name, std::ios::in | std::ios::binary);
  if (!stream) {
    ERROR(" Couldn't open file " << name << "!" << std::endl);
  }
  stream.seekg(0, std::ios::end);
  std::string res;
  size_t len = stream.tellg();
  res.resize(len);
  stream.seekg(0, std::ios::beg);
  stream.read(&res[0], len);
  stream.close();
  return res;
}

#include <boost/filesystem.hpp>

std::string resolveCmdLineArgPath(std::string relative) {
  return boost::filesystem::absolute(relative).string();
}

std::string resolvePath(std::string baseLoc, std::string relative) {
  auto basePath = boost::filesystem::path(baseLoc).parent_path();
  auto relativePath = boost::filesystem::path(relative);
  return boost::filesystem::absolute(relativePath, basePath).string();
}

void readAnotherFile(NormalWriter &r, const std::string t) {
  std::string fileBefore = curFile;
  curFile = resolvePath(fileBefore, t);
  Parser<NormalWriter> p(readFile(curFile), r);
  p();
  curFile = fileBefore;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "USAGE: SOURCE_FILE TARGET_FILE" << std::endl;
    exit(1);
  }

  curFile = resolveCmdLineArgPath(argv[1]);
  auto a = Parser<NormalWriter>::parse(readFile(curFile))();
  std::ofstream f(argv[2], std::ios::binary);
  Serializer::Write(a, f);

  return 0;
}

#endif
