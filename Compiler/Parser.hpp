#ifndef INCLUDED_Parser_hpp
#define INCLUDED_Parser_hpp

class NormalWriter;

#include <string>
#include <vector>
#include <utility>
#include <Algorithms.hpp>
#include <Error.hpp>
#include <TextAdventure.hpp>
#include "CodeStream.hpp"

// abbreviation
using string_vector = std::vector<std::string>;

void readAnotherFile(NormalWriter &r, const std::string t); // from Starter.cpp

struct ParsedChoiceBoxEntry {
  std::string Choice;
  std::string Text;
  string_vector Cond;
  std::vector<std::pair<std::string, string_vector>> Cmds;
};

class NormalWriter {
  TextAdventure _ta;
  string_vector _verbs;
  string_vector _nouns;
  string_vector _choiceboxes;

public:
  NormalWriter() {
    // creating pseudo verbs
    _ta.Verbs = {{{}, 0}, {{}, 0}};
    _verbs = {"_inv", "_intro"};
  }

  void writeCommand(std::string name, string_vector args) {
    if (name == "inc") {
      if (args.size() != 1) {
        ERROR("inc: Expected exactly 1 argument.");
      }
      readAnotherFile(*this, args[0]);
    } else if (name == "verb") {
      if (args.size() != 3) {
        ERROR("verb: Expected exactly 3 arguments.");
      }
      auto r = find(_verbs, args[1]);
      // TODO: Maybe don't trust the user in giving correct ArgCounts
      if (r == notFound) {
        _verbs.push_back(args[1]);
        _ta.Verbs.push_back(
            {{args[0]}, static_cast<int8_t>(std::stoi(args[2]))});
      } else {
        _ta.Verbs.at(r).Regexes.push_back(args[1]);
      }
    } else if (name == "inventory") {
      if (args.size() != 2) {
        ERROR("inventory: Expected exactly 2 arguments.");
      }
      _nouns.push_back(args[1]);
      _ta.Nouns.push_back({Noun::InventoryItem, -1, args[0]});
    } else if (name == "var") {
      if (args.size() != 1) {
        ERROR("var: Expected exactly 1 argument.");
      }
      _nouns.push_back(args[0]);
      _ta.Nouns.push_back({Noun::Variable, -1, std::string()});
    } else if (name == "room") {
      if (args.size() != 2) {
        ERROR("room: Expected exactly 2 arguments.");
      }
      _nouns.push_back(args[1]);
      _ta.Nouns.push_back({Noun::Room, -1, args[0]});
    } else if (name == "object") {
      if (args.size() != 3) {
        ERROR("object: Expected exactly 2 arguments.");
      }
      auto r = find(_nouns, args[0]);
      if (r == notFound) {
        ERROR("object: Room couldn't be found: " << args[0] << "!");
      }
      _nouns.push_back(args[2]);
      _ta.Nouns.push_back({Noun::RoomItem, static_cast<ID>(r), args[1]});
    } else {
      ERROR("Unknown Command: " << name);
    }
  }

  Command writeActionCmd(std::string s, string_vector args) {
    if (s == "get" || s == "set") {
      if (args.size() != 1) {
        ERROR("get/set: Expected exactly 1 argument.");
      }
      auto r = find(_nouns, args[0]);
      if (r == notFound) {
        ERROR("get/set: Couldn't find " << args[0] << "!");
      }
      return {Command::get, static_cast<ID>(r), 0};
    } else if (s == "lose" || s == "unset") {
      if (args.size() != 1) {
        ERROR("lose/unset: Expected exactly 1 argument.");
      }
      auto r = find(_nouns, args[0]);
      if (r == notFound) {
        ERROR("lose/unset: Couldn't find " << args[0] << "!");
      }
      return {Command::lose, static_cast<ID>(r), 0};
    } else if (s == "choicebox") {
      if (args.size() != 1) {
        ERROR("choicebox: Expected exactly 1 argument.");
      }
      auto r = find(_choiceboxes, args[0]);
      if (r == notFound) {
        ERROR("choicebox: Couldn't find " << args[0] << "!");
      }
      return {Command::choicebox, static_cast<ID>(r), 0};
    } else if (s == "leave") {
      if (args.size() != 0) {
        ERROR("leave: Expected exactly 0 arguments.");
      }
      return {Command::leave, 0, 0};
    } else if (s == "go") {
      if (args.size() != 1) {
        ERROR("go: Expected exactly 1 argument.");
      }
      auto r = find(_nouns, args[0]);
      if (r == notFound) {
        ERROR("go: Couldn't find " << args[0] << "!");
      }
      return {Command::go, static_cast<ID>(r), 0};
    } else if (s == "end") {
      if (args.size() != 0) {
        ERROR("object: Expected exactly 0 arguments.");
      }
      return {Command::end, 0, 0};
    }
    ERROR("Couldn't find action command: " << s << "!");
  }

  void writeAction(string_vector objects_, std::string verb_,
                   string_vector cond_, std::string text,
                   std::vector<std::pair<std::string, string_vector>> cmds_) {
    const auto func = [this](auto a) {
      auto r = find(_nouns, a);
      if (r == notFound) {
        ERROR("Couldn't find " << a << "!");
      }
      return r;
    };

    // Selector
    auto objects = map<ID>(objects_, func);
    auto v = find(_verbs, verb_);
    if (v == notFound) {
      ERROR("Couldn't find verb " << verb_ << "!");
    }
    ID verb = v;
    uint16_t i = 1;
    std::vector<Condition> cond;
    for (const auto &e : cond_) {
      if (e.size()) {
        if (e[0] >= '0' && e[0] <= '9') {
          i = std::stoi(e);
        } else {
          Condition res;
          if (e[0] == '!') {
            res.expectedValue = false;
            res.var = func(e.substr(1));
          } else {
            res.expectedValue = true;
            res.var = func(e);
          }
          cond.push_back(res);
        }
      }
    }
    ActionSelector sel = {verb, objects, i, cond};

    // text is already there

    // Commands
    auto cmds = map<Command>(cmds_, [this](auto a) {
      return this->writeActionCmd(a.first, a.second);
    });

    _ta.Actions.push_back({sel, text, cmds});
  }

  Choice writeChoice(ParsedChoiceBoxEntry entry) {
    const auto func = [this](auto a) {
      auto r = find(_nouns, a);
      if (r == notFound) {
        ERROR("Couldn't find " << a << "!");
      }
      return r;
    };

    // I, Conditions
    uint16_t i = 1;
    std::vector<Condition> cond;
    for (const auto &e : entry.Cond) {
      if (e.size()) {
        if (e[0] >= '0' && e[0] <= '9') {
          i = std::stoi(e);
        } else {
          Condition res;
          if (e[0] == '!') {
            res.expectedValue = false;
            res.var = func(e.substr(1));
          } else {
            res.expectedValue = true;
            res.var = func(e);
          }
          cond.push_back(res);
        }
      }
    }

    // Choice, Text already there

    // Commands
    auto cmds = map<Command>(entry.Cmds, [this](auto a) {
      return this->writeActionCmd(a.first, a.second);
    });

    return {i, cond, entry.Choice, entry.Text, cmds};
  }

  void writeChoiceBox(std::string name,
                      std::vector<ParsedChoiceBoxEntry> entries) {
    auto choices =
        map<Choice>(entries, [this](auto a) { return this->writeChoice(a); });
    _ta.ChoiceBoxes.push_back({choices});
    _choiceboxes.push_back(name);
  }

  TextAdventure operator()() { return _ta; }
};

template <class TWriter> class Parser {
  CodeStream _c;
  TWriter &_w;

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

  string_vector parseArgs(char end) {
    string_vector res;
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

  std::pair<std::string, string_vector> parseStrFunction() {
    std::string name = parseName('(');
    auto args = parseArgs(')');
    return {name, args};
  }

  std::pair<std::string, std::vector<std::pair<std::string, string_vector>>>
  parseString() {
    std::vector<std::pair<std::string, string_vector>> func;
    std::string res;
    for (;;) {
      if (_c.eof()) {
        return {trim(res), func};
      }
      auto a = _c.next();
      if (a == '+' || a == '$' || a == '#' || a == '*') {
        _c.rewind();
        return {trim(res), func};
      }
      if (a == '@') {
        func.push_back(parseStrFunction());
        continue;
      }
      res += a;
    }
  }

  string_vector parseNames(char begin) {
    string_vector res;
    std::string s;
    for (;;) {
      if (_c.eof()) {
        ERROR("Unexpected EOF!");
      }
      char a = _c.next();
      if (a == ' ') {
        ERROR("Unexpected whitespace!");
      }
      if (a == '-') {
        res.push_back(s);
        s = "";
        continue;
      }
      if (a == begin) {
        res.push_back(s);
        return res;
      }
      s += a;
    }
  }

  std::string parseCBEntry() {
    std::string res;
    for (;;) {
      if (_c.eof()) {
        return trim(res);
      }
      auto a = _c.next();
      if (a == '|') {
        return trim(res);
      }
      res += a;
    }
  }

  void parseCommand() {
    std::string name = parseName('(');
    auto args = parseArgs(')');
    _w.writeCommand(name, args);
  }

  void parseAction() {
    auto names = parseNames('(');
    auto args = parseArgs(')');
    if (args.size() != 1) {
      ERROR("Expected EXACTLY one verb, found " << args.size());
    }
    string_vector args2;
    if (_c.next() != ':') {
      args2 = parseArgs(']');
    }
    while (_c.last != ':') {
      if (_c.eof()) {
        ERROR("Unexpected EOF!");
      }
      _c.next();
    }
    auto text = parseString();
    _w.writeAction(names, args[0], args2, text.first, text.second);
  }

  void parseChoiceBox() {
    auto name = parseName(':');
    std::vector<ParsedChoiceBoxEntry> entries;
    for (;;) {
      while (_c.last != '*') {
        if (_c.eof()) {
          ERROR("Unexpected EOF!");
        }
        _c.next();
      }
      string_vector args;
      if (_c.next() != ':') {
        args = parseArgs(']');
        _c.next(); // jump over
      }
      auto entry = parseCBEntry();
      auto str = parseString();
      entries.push_back({entry, str.first, args, str.second});
      auto a = _c.next();
      if (a == '$' || a == '+' || a == '#') {
        _c.rewind();
        _w.writeChoiceBox(name, entries);
        return;
      }
    }
  }

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
    default:
      ERROR("Unexpected character: " << cmdType);
    }
  }

public:
  Parser(std::string s, TWriter &t) : _c(s), _w(t) {}

  void operator()() {
    while (!_c.eof()) {
      parseHighLevel();
    }
  }

  static TWriter parse(const std::string t) {
    TWriter a;
    Parser<TWriter> p(t, a);
    p();
    return a;
  }
};

#endif
