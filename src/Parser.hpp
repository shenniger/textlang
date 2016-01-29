#ifndef INCLUDED_Parser_hpp
#define INCLUDED_Parser_hpp

#include <string>
#include <vector>
#include "Error.hpp"
#include "TextAdventure.hpp"
#include "CodeStream.hpp"
#include "Unittest.hpp"

class Parser {
  CodeStream _c;

  std::string trimBegin(const std::string &a) {
    if (a.size()) {
      if (a[0] == ' ') {
        return a.substr(1);
      }
      return a;
    }
    return a;
  }
  std::string trimEnd(const std::string &a) {
    if (a.size()) {
      if (a[a.size() - 1] == ' ') {
        return a.substr(0, a.size() - 1);
      }
      return a;
    }
    return a;
  }
  std::string trim(const std::string &a) { return trimBegin(trimEnd(a)); }

  std::string parseName(char begin) {
    std::string res;
    for (;;) {
      if (_c.eof()) {
        ERROR("Unexpected EOF!");
      }
      char a = _c.next();
      if (a == ' ') {
        ERROR("Unexpected whitespace!");
      }
      if (a == begin)
        return res;
      res += a;
    }
  }

  std::string parseArg(char end) {
    std::string res;
    for (;;) {
      if (_c.eof()) {
        ERROR("Unexpected EOF!");
      }
      _c.next();
      if (_c.last == end || _c.last == ',') {
        return res;
      }
      res += _c.last;
    }
  }

  std::vector<std::string> parseArgs(char end) {
    std::vector<std::string> res;
    for (;;) {
      if (_c.eof()) {
        ERROR("Unexpected EOF!");
      }
      if (_c.last == end) {
        return res;
      }
      auto arg = parseArg(end);
      if (arg.size())
        res.push_back(trim(arg));
    }
  }

  void parseCommand() {
    std::string name = parseName('(');
    auto args = parseArgs(')');
    std::cout << name << "->"; // TODO
    for (auto &e : args) {
      std::cout << e << "|";
    }
    std::cout << "\n";
  }

  void parseAction() {}

  void parseChoiceBox() {}

  void parseHighLevel() {
    char cmdType = _c.next();
    if (cmdType == ' ') {
      if (_c.eof()) {
        return;
      }
      cmdType = _c.next();
    }
    switch (cmdType) {
    case '+':
      parseCommand();
      break;
    case '$':
      parseAction();
      break;
    case '#':
      parseChoiceBox();
      break;
    }
  }

public:
  Parser(std::string s) : _c(s) {}

  void /* TODO */ operator()() {
    while (!_c.eof()) {
      parseHighLevel();
    }
  }
};

#if (defined UNITTEST) || (defined COMPLETION)
DEF_UNITTEST(Parser) {
  Parser(R"(

\ unittest \

+test(1, 2, 3, "fnb")

)")();
}
END_UNITTEST
#endif
#endif
