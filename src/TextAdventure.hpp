#ifndef INCLUDED_TextAdventure_hpp
#define INCLUDED_TextAdventure_hpp

struct Verb;
struct Noun;
struct NounSelector;
struct Condition;
struct ActionSelector;
struct Command;
struct Action;
struct TextAdventure;

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>
#include <utility>

using ID = int16_t;

struct Verb {
  std::vector<std::string> Regexes;
  int8_t NounCount;
};

struct Noun {
  enum Type : uint8_t { Room, RoomItem, InventoryItem, Variable };
  Type T;
  ID InRoom;         // -1 if non-room dependant
  std::string Title; // not neccessary if Type==Variable
};

struct Condition {
  bool expectedValue;
  ID var;
};

struct ActionSelector {
  ID Verb;
  std::vector<ID> Nouns;
  uint16_t I;
  std::vector<Condition> Conditions;
};

struct Command {
  enum Type : uint8_t {};
  Type T;
  ID Arg1, Arg2;
};

struct Action {
  ActionSelector Selector;
  std::string Result;
  std::vector<Command> Commands;
};

struct TextAdventure {
  std::vector<Verb> Verbs;
  std::vector<Noun> Nouns;
  std::vector<Action> Actions;
};

#endif
