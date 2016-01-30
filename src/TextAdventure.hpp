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
  enum Type : uint8_t {
    get,       // get Item, same as set
    lose,      // lose Item, same as unset
    choicebox, // enter choice box
    leave,     // leave choice box
    go         // change location
  };
  Type T;
  ID Arg1, Arg2;
};

struct Action {
  ActionSelector Selector;
  std::string Result;
  std::vector<Command> Commands;
};

struct Choice {
  uint16_t I;
  std::vector<Condition> Conditions;

  std::string Choice;
  std::string Text;

  std::vector<Command> Commands;
};

struct ChoiceBox {
  std::vector<Choice> C;
};

struct TextAdventure {
  std::vector<Verb> Verbs;
  std::vector<Noun> Nouns;
  std::vector<Action> Actions;
  std::vector<ChoiceBox> ChoiceBoxes;
};

#endif
