#ifndef INCLUDED_TextEngine_hpp
#define INCLUDED_TextEngine_hpp

#include <stddef.h>
#include <stdint.h>
#include <regex>
#include <stack>
#include <string>
#include <textlang/Internal/Algorithms.hpp>
#include <textlang/Internal/TextAdventure.hpp>
#include <textlang/Serialization.hpp>
#include <utility>
#include <vector>

class TextEngine {
 public:
  struct Answer {
    std::string Text;
    ID ClientAction = -1;
    ID ChoiceBoxIndex = -1;
    std::vector<std::pair<std::string, ID>> Choices;

    void add(const Answer &o) {
      Text += o.Text;
      if (o.ClientAction != -1) {
        ClientAction = o.ClientAction;
      }
      if (o.ChoiceBoxIndex != -1) {
        ChoiceBoxIndex = o.ChoiceBoxIndex;
        Choices = o.Choices;
      }
    }
  };

  struct State {
    std::vector<uint16_t> ActI;
    std::vector<bool> NounVals;
    ID Room;

    SERIALIZE(&ActI &NounVals &Room)
  };

 private:
  TextAdventure _ta;
  State _s;
  std::vector<std::vector<std::regex>> _verbs;

  std::stack<ID> _choiceBoxes;  // this is not included in State as it's empty
                                // when the game is being saved

  template <class T>
  static std::vector<T> removeFirstElement(const std::vector<T> &v) {
    return v.size() < 1 ? std::vector<T>()
                        : std::vector<T>(v.begin() + 1, v.end());
  }

  Action genInv() const {
    Action res;
    auto itState = _s.NounVals.cbegin();
    auto itAssets = _ta.Nouns.cbegin();
    while (itState != _s.NounVals.end()) {
      if ((*itState) && itAssets->T == Noun::InventoryItem) {
        res.Result += "  " + itAssets->Title + "\n";
      }

      itState++;
      itAssets++;
    }
    return res;
  }

  Action findAction(const std::pair<ID, std::vector<ID>> &query) const {
    if (query.first == 0) {  // _inv
      return genInv();
    }
    Action res;
    uint16_t greatestVal = 0;
    size_t i = 0;
    bool found = false;
    for (const auto &e : _ta.Actions) {
      if (e.Selector.matches(query.first, query.second, _s.NounVals) &&
          _s.ActI.at(i) >= e.Selector.I && e.Selector.I > greatestVal) {
        found = true;
        greatestVal = e.Selector.I;
        res = e;
      }
      i++;
    }
    if (!found) return findAction({2, {0}});
    return res;
  }

  std::pair<ID, std::vector<std::string>> processQuery(
      const std::string &text) {
    ID i = 0;
    std::smatch matches;
    for (const auto &e : _verbs) {
      for (const auto &ele : e) {
        if (std::regex_match(text, matches, ele)) {
          return {i, removeFirstElement(map<std::string>(
                         matches, [](auto a) { return a.str(); }))};
        }
      }
      i++;
    }
    return {-1, {}};
  }

  std::vector<std::pair<std::string, ID>> doChoiceBox(const ID num) {
    std::vector<std::pair<std::string, ID>> res;
    ID i = 0;
    for (const auto &e : _ta.ChoiceBoxes.at(num).C) {
      bool viable = true;
      for (const auto &eCond : e.Conditions) {
        if (!eCond.matches(_s.NounVals)) viable = false;
      }
      if (viable) {
        res.push_back({e.Choice, i});
      }
      i++;
    }
    return res;
  }

  void doCommand(const Command cmd, Answer &ans) {
    switch (cmd.T) {
      case Command::get:
        _s.NounVals.at(cmd.Args.at(0)) = true;
        break;
      case Command::lose:
        _s.NounVals.at(cmd.Args.at(0)) = false;
        break;
      case Command::go: {
        _s.NounVals.at(_s.Room) = false;
        _s.NounVals.at(cmd.Args.at(0)) = true;
        _s.Room = cmd.Args.at(0);

        Answer newAns = execAction(findAction({1, {_s.Room}}));
        if (ans.Text.empty() || newAns.Text.empty()) {
          ans.Text += newAns.Text;
        } else {
          ans.Text += "\n" + newAns.Text;
        }
        if (newAns.ChoiceBoxIndex != -1) {
          ans.ChoiceBoxIndex = newAns.ChoiceBoxIndex;
          ans.Choices = newAns.Choices;
        }
        if (newAns.ClientAction != -1) {
          ans.ClientAction = newAns.ClientAction;
        }
        break;
      }
      case Command::choicebox:
        ans.ChoiceBoxIndex = cmd.Args.at(0);
        ans.Choices = doChoiceBox(cmd.Args.at(0));
        _choiceBoxes.push(ans.ChoiceBoxIndex);
        break;
      case Command::leave:
        // ignoring leave here, handled below
        break;
      case Command::call: {
        std::pair<ID, std::vector<ID>> query = {
            cmd.Args.at(0), {cmd.Args.begin() + 1, cmd.Args.end()}};
        auto otherAns = execAction(findAction(query));
        ans.add(otherAns);
        break;
      }
      case Command::client:
        ans.ClientAction = cmd.Args.at(0);
        break;
    }
  }

  Answer execAction(const Action action) {
    Answer ans;
    ans.Text = action.Result;

    // incrementing I
    auto itActI = _s.ActI.begin();
    auto itActions = _ta.Actions.begin();
    while (itActI != _s.ActI.end()) {
      if (itActions->Selector == action.Selector) {
        (*itActI)++;
      }

      itActI++;
      itActions++;
    }

    for (const auto &e : action.Commands) doCommand(e, ans);
    return ans;
  }

 public:
  TextEngine() = default;
  TextEngine(const TextAdventure ta)
      : _ta(ta),
        _s{std::vector<uint16_t>(_ta.Actions.size(), 1),
           map<bool>(_ta.Nouns, [](auto a) { return a.T == Noun::RoomItem; }),
           1},
        _verbs{map<std::vector<std::regex>>(_ta.Verbs, [](Verb a) {
          return map<std::regex>(a.Regexes, [](auto a) {
            return std::regex(a, std::regex_constants::icase);
          });
        })} {}

  Answer begin() {
    _s.NounVals.at(_s.Room) = true;

    return execAction(findAction({1, {_s.Room}}));
  }

  Answer query(const std::string text) {
    auto verbs = processQuery(text);
    std::pair<ID, std::vector<ID>> query;
    if (verbs.first == -1) {
      query.first = 2;  // _unk
      query.second = {};
    } else {
      query = {verbs.first, map<ID>(verbs.second, [this](auto a) -> ID {
                 ID res = 0;
                 for (auto &e : _ta.Nouns) {
                   for (auto &f : e.Aliases) {
                     if (a == f && ((e.T != Noun::InventoryItem &&
                                     e.T != Noun::RoomItem) ||
                                    _s.NounVals[res]) &&
                         (e.InRoom == -1 || e.InRoom == _s.Room))
                       return res;
                   }
                   res++;
                 }
                 return -1;
               })};
      for (auto a : query.second) {
        if (a == -1) {
          query.second = {0};
          break;
        }
      }
    }
    return execAction(findAction(query));
  }

  Answer choiceBoxQuery(const ID ChoiceBox, const ID Option) {
    auto entry = _ta.ChoiceBoxes.at(ChoiceBox).C.at(Option);
    Answer ans;
    ans.ChoiceBoxIndex = ChoiceBox;
    for (const auto &e : entry.Commands) {
      if (e.T == Command::leave) {
        _choiceBoxes.pop();
        if (_choiceBoxes.empty()) {
          ans.ChoiceBoxIndex = -1;
        } else {
          ans.ChoiceBoxIndex = _choiceBoxes.top();
        }
      }
      doCommand(e, ans);
    }
    ans.Text = entry.Text;
    if (ans.ChoiceBoxIndex != -1) {
      ans.Choices = doChoiceBox(ans.ChoiceBoxIndex);
    }
    return ans;
  }

  State &state() { return _s; }
};

#endif
