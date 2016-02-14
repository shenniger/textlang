#ifndef INCLUDED_Parser_hpp
#define INCLUDED_Parser_hpp

class NormalWriter;

#include <string>
#include <vector>
#include <utility>
#include <algorithm>
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
  std::vector<std::pair<std::string, ID>> _verbs;
  string_vector _nouns;
  string_vector _choiceboxes;

public:
  NormalWriter() {
    // creating pseudo verbs
    _ta.Verbs = {{{}}, {{}}, {{}}};
    _verbs = {{"_inv", 0}, {"_intro", 1}, {"_unk", -1}};
    _ta.Nouns = {{Noun::RoomItem, -1, "", {}}};
    _nouns = {"_unk"};
  }

  std::string replaceAll(const std::string &haystack, const std::string &needle,
                         const std::string &to) {
    if (haystack.empty())
      return haystack;
    std::string res{haystack};
    size_t pos = 0;
    while ((pos = res.find(needle, pos)) != std::string::npos) {
      res.replace(pos, needle.length(), to);
      pos += to.length();
    }
    return res;
  }

  void writeCommand(int line, std::string name, string_vector args) {
    if (name == "inc") {
      if (args.size() != 1) {
        ERROR(line << ": inc: Expected exactly 1 argument.");
      }
      readAnotherFile(*this, args[0]);
    } else if (name == "verb") {
      if (args.size() != 2) {
        ERROR(line << ": verb: Expected exactly 2 arguments.");
      }
      auto r = find(_verbs, [s = args[1]](auto a) { return s == a.first; });
      ID argCount = std::count(args[0].begin(), args[0].end(), '&');
      if (r == notFound) {
        _verbs.push_back({args[1], argCount});
        _ta.Verbs.push_back({{replaceAll(args[0], "&", "(.*)")}});
      } else {
        if (argCount != _verbs.at(r).second) {
          ERROR(line << ": verb: Argument count differs for nouns "
                     << _verbs.at(r).first << "!");
        }
        _ta.Verbs.at(r).Regexes.push_back(replaceAll(args[0], "&", "(.*)"));
      }
    } else if (name == "inventory") {
      if (args.size() != 2) {
        ERROR(line << ": inventory: Expected exactly 2 arguments.");
      }
      _nouns.push_back(args[1]);
      _ta.Nouns.push_back({Noun::InventoryItem, -1, args[0], {}});
    } else if (name == "var") {
      if (args.size() != 1) {
        ERROR(line << ": var: Expected exactly 1 argument.");
      }
      _nouns.push_back(args[0]);
      _ta.Nouns.push_back({Noun::Variable, -1, std::string(), {}});
    } else if (name == "room") {
      if (args.size() != 2) {
        ERROR(line << ": room: Expected exactly 2 arguments.");
      }
      _nouns.push_back(args[1]);
      _ta.Nouns.push_back({Noun::Room, -1, args[0], {}});
    } else if (name == "object") {
      if (args.size() != 3) {
        ERROR(line << ": object: Expected exactly 2 arguments.");
      }
      auto r = find(_nouns, args[0]);
      if (r == notFound) {
        ERROR(line << ": object: Room couldn't be found: " << args[0] << "!");
      }
      _nouns.push_back(args[2]);
      _ta.Nouns.push_back({Noun::RoomItem, static_cast<ID>(r), args[1], {}});
    } else if (name == "noun") {
      if (args.size() != 2) {
        ERROR(line << ": noun: Expected exactly 2 arguments.");
      }
      _nouns.push_back(args[1]);
      _ta.Nouns.push_back({Noun::RoomItem, -1, args[0], {}});
    } else if (name == "alias") {
      if (args.size() != 2) {
        ERROR(line << ": alias: Expected exactly 2 arguments.");
      }
      auto r = find(_nouns, args[0]);
      if (r == notFound) {
        ERROR(line << ": alias: Noun couldn't be found: " << args[0] << "!");
      }
      _ta.Nouns.at(r).Aliases.push_back(args[1]);
    } else {
      ERROR(line << ": Unknown Command: " << name);
    }
  }

  Command writeActionCmd(int line, std::string s, string_vector args) {
    if (s == "get" || s == "set") {
      if (args.size() != 1) {
        ERROR(line << ": get/set: Expected exactly 1 argument.");
      }
      auto r = find(_nouns, args[0]);
      if (r == notFound) {
        ERROR(line << ": get/set: Couldn't find " << args[0] << "!");
      }
      return {Command::get, static_cast<ID>(r), 0};
    } else if (s == "lose" || s == "unset") {
      if (args.size() != 1) {
        ERROR(line << ": lose/unset: Expected exactly 1 argument.");
      }
      auto r = find(_nouns, args[0]);
      if (r == notFound) {
        ERROR(line << ": lose/unset: Couldn't find " << args[0] << "!");
      }
      return {Command::lose, static_cast<ID>(r), 0};
    } else if (s == "choicebox") {
      if (args.size() != 1) {
        ERROR(line << ": choicebox: Expected exactly 1 argument.");
      }
      auto r = find(_choiceboxes, args[0]);
      if (r == notFound) {
        ERROR(line << ": choicebox: Couldn't find " << args[0] << "!");
      }
      return {Command::choicebox, static_cast<ID>(r), 0};
    } else if (s == "leave") {
      if (args.size() != 0) {
        ERROR(line << ": leave: Expected exactly 0 arguments.");
      }
      return {Command::leave, 0, 0};
    } else if (s == "go") {
      if (args.size() != 1) {
        ERROR(line << ": go: Expected exactly 1 argument.");
      }
      auto r = find(_nouns, args[0]);
      if (r == notFound) {
        ERROR(line << ": go: Couldn't find " << args[0] << "!");
      }
      return {Command::go, static_cast<ID>(r), 0};
    } else if (s == "end") {
      if (args.size() != 0) {
        ERROR(line << ": object: Expected exactly 0 arguments.");
      }
      return {Command::end, 0, 0};
    }
    ERROR(line << ": Couldn't find action command: " << s << "!");
  }

  void writeAction(int line, string_vector objects_, std::string verb_,
                   string_vector cond_, std::string text,
                   std::vector<std::pair<std::string, string_vector>> cmds_) {
    const auto func = [this, line](auto a) {
      auto r = find(_nouns, a);
      if (r == notFound) {
        ERROR(line << ": Couldn't find " << a << "!");
      }
      return r;
    };

    // Selector
    auto objects = map<ID>(objects_, func);
    std::sort(objects.begin(), objects.end());
    auto v = find(_verbs, [verb_](auto a) { return verb_ == a.first; });
    if (v == notFound) {
      ERROR(line << ": Couldn't find verb " << verb_ << "!");
    }
    ID verb = v;
    if (_verbs.at(verb).second != static_cast<ID>(objects.size()) &&
        _verbs.at(verb).second != -1) {
      ERROR(line << ": Expected " << _verbs.at(verb).second
                 << " nouns for verb " << _verbs.at(verb).first << ", found "
                 << objects.size() << "!");
    }
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
    auto cmds = map<Command>(cmds_, [this, line](auto a) {
      return this->writeActionCmd(line, a.first, a.second);
    });

    _ta.Actions.push_back({sel, text, cmds});
  }

  Choice writeChoice(int line, ParsedChoiceBoxEntry entry) {
    const auto func = [this, line](auto a) {
      auto r = find(_nouns, a);
      if (r == notFound) {
        ERROR(line << ": Couldn't find " << a << "!");
      }
      return r;
    };

    // I, Conditions
    std::vector<Condition> cond;
    for (const auto &e : entry.Cond) {
      if (e.size()) {
        if (e[0] >= '0' && e[0] <= '9') {
          ERROR(line << ": i not supported in ChoiceBoxes!");
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
    auto cmds = map<Command>(entry.Cmds, [this, line](auto a) {
      return this->writeActionCmd(line, a.first, a.second);
    });

    return {cond, entry.Choice, entry.Text, cmds};
  }

  void writeChoiceBox(int line, std::string name,
                      std::vector<ParsedChoiceBoxEntry> entries) {
    auto choices = map<Choice>(
        entries, [this, line](auto a) { return this->writeChoice(line, a); });
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
        ERROR(_c.lineCount << ": Unexpected EOF!");
      }
      char a = _c.next();
      if (a == ' ') {
        ERROR(_c.lineCount << ": Unexpected whitespace!");
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
        ERROR(_c.lineCount << ": Unexpected EOF!");
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
        ERROR(_c.lineCount << ": Unexpected EOF!");
      }
      if (_c.last == end) {
        return res;
      }
      auto arg = parseArg(end);
      if (!arg.empty())
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
        ERROR(_c.lineCount << ": Unexpected EOF!");
      }
      char a = _c.next();
      if (a == ' ') {
        ERROR(_c.lineCount << ": Unexpected whitespace!");
      }
      if (a == '-') {
        res.push_back(s);
        s = "";
        continue;
      }
      if (a == begin) {
        if (!s.empty())
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
    _w.writeCommand(_c.lineCount, name, args);
  }

  void parseAction() {
    auto names = parseNames('(');
    auto args = parseArgs(')');
    if (args.size() != 1) {
      ERROR(_c.lineCount << ": Expected EXACTLY one verb, found "
                         << args.size());
    }
    string_vector args2;
    if (_c.next() != ':') {
      args2 = parseArgs(']');
    }
    while (_c.last != ':') {
      if (_c.eof()) {
        ERROR(_c.lineCount << ": Unexpected EOF!");
      }
      _c.next();
    }
    auto text = parseString();
    _w.writeAction(_c.lineCount, names, args[0], args2, text.first,
                   text.second);
  }

  void parseChoiceBox() {
    auto name = parseName(':');
    std::vector<ParsedChoiceBoxEntry> entries;
    for (;;) {
      while (_c.last != '*') {
        if (_c.eof()) {
          ERROR(_c.lineCount << ": Unexpected EOF!");
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
        _w.writeChoiceBox(_c.lineCount, name, entries);
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
      ERROR(_c.lineCount << ": Unexpected character: " << cmdType);
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
