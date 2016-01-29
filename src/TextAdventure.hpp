#ifndef INCLUDED_TextAdventure_hpp
#define INCLUDED_TextAdventure_hpp

struct Verb;
struct Noun;
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

struct Verb {
  std::vector<std::string> Regex;
  uint8_t NounNum;
};

struct Noun {
  enum Type : uint8_t { Room, RoomItem, InventoryItem };
  Type T;
  int16_t InRoom; // -1 if non-room dependant
  std::vector<std::string> Name;
};

struct Condition {
  bool expectedValue;
  int16_t var;
};

struct ActionSelector {
  int16_t Room;
  std::vector<int16_t> Nouns;
  uint16_t I;
  std::vector<Condition> Conditions;
};

struct Command {
  enum Type : uint8_t {};
  Type T;
  int16_t Arg;
};

struct Action {
  ActionSelector Selector;
  std::string Result;
  std::vector<Command> Commands;
};

struct TextAdventure {
  int16_t varCount;
  std::vector<Verb> Verbs;
  std::vector<Noun> Nouns;
  std::vector<Action> Actions;
};

#endif
