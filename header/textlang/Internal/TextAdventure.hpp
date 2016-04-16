#ifndef INCLUDED_TextAdventure_hpp
#define INCLUDED_TextAdventure_hpp

struct Verb;
struct Noun;
struct NounSelector;
struct Condition;
struct ActionSelector;
struct Command;
struct Action;
struct Choice;
struct ChoiceBox;
struct TextAdventure;

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>
#include <textlang/Serialization.hpp>

using ID = int16_t;

struct Verb {
  std::vector<std::string> Regexes;

  SERIALIZE(&Regexes)
};

struct Noun {
  enum Type : uint8_t { Room, RoomItem, InventoryItem, Variable };
  Type T;
  ID InRoom;         // -1 if non-room dependant
  std::string Title; // not neccessary if Type==Variable
  std::vector<std::string> Aliases;

  SERIALIZE(&T &InRoom &Title &Aliases)
};

struct Condition {
  bool expectedValue;
  ID var;

  bool operator==(const Condition &rhs) const {
    return expectedValue == rhs.expectedValue && var == rhs.var;
  }

  bool matches(const std::vector<bool> &c) const {
    return c.at(var) == expectedValue;
  }

  SERIALIZE(&expectedValue &var)
};

struct ActionSelector {
  ID Verb;
  std::vector<ID> Nouns;
  uint16_t I;
  std::vector<Condition> Conditions;

  // does NOT respect I!
  bool operator==(const ActionSelector &rhs) const {
    return Verb == rhs.Verb && Nouns == rhs.Nouns &&
           Conditions == rhs.Conditions;
  }

  bool matches(const ID v, const std::vector<ID> n,
               const std::vector<bool> &c) const {
    std::vector<ID> a(n);
    std::sort(a.begin(), a.end());
    if (!(v == Verb && Nouns == a))
      return false;
    for (auto e : Conditions) {
      if (!e.matches(c))
        return false;
    }
    return true;
  }

  SERIALIZE(&Verb &Nouns &I &Conditions)
};

struct Command {
  enum Type : uint8_t {
    get,       // get Item, same as set
    lose,      // lose Item, same as unset
    choicebox, // enter choice box
    leave,     // leave choice box
    go,        // change location
    end        // ends game
  };
  Type T;
  ID Arg1, Arg2;

  SERIALIZE(&T &Arg1 &Arg2)
};

struct Action {
  ActionSelector Selector;
  std::string Result;
  std::vector<Command> Commands;

  SERIALIZE(&Selector &Result &Commands)
};

struct Choice {
  std::vector<Condition> Conditions;

  std::string Choice;
  std::string Text;

  std::vector<Command> Commands;

  SERIALIZE(&Conditions &Choice &Text &Commands)
};

struct ChoiceBox {
  std::vector<Choice> C;

  SERIALIZE(&C)
};

struct TextAdventure {
  std::vector<Verb> Verbs;
  std::vector<Noun> Nouns;
  std::vector<Action> Actions;
  std::vector<ChoiceBox> ChoiceBoxes;

  SERIALIZE(&Verbs &Nouns &Actions &ChoiceBoxes)
};

#endif
