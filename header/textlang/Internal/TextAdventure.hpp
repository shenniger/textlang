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

#include <algorithm>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <textlang/Serialization.hpp>
#include <utility>
#include <vector>

using ID = int16_t;

struct Verb {
  std::vector<std::string> Regexes;

  SERIALIZE(&Regexes)
};

struct Noun {
  enum Type : uint8_t { Room, RoomItem, InventoryItem, Variable };
  Type T;
  ID InRoom;          // -1 if non-room dependant
  std::string Title;  // not neccessary if Type==Variable
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
    if (!(v == Verb && Nouns == a)) return false;
    for (auto e : Conditions) {
      if (!e.matches(c)) return false;
    }
    return true;
  }

  SERIALIZE(&Verb &Nouns &I &Conditions)
};

struct Command {
  using Arguments = std::basic_string<ID>;
  enum Type : uint8_t {
    get,        // get Item, same as set and reactivate
    lose,       // lose Item, same as unset and deactivate
    choicebox,  // enter choice box
    leave,      // leave choice box
    go,         // change location
    call,       // call other action
    client      // runs client action (-1 reserved, 0 for ending game,
                //                     1-64 reserved)
  };
  Type T;
  Arguments Args;

  SERIALIZE(&T &Args)
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
