#ifndef INCLUDED_TextEngine_hpp
#define INCLUDED_TextEngine_hpp

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <regex>
#include <utility>
#include <stack>
#include <Algorithms.hpp>
#include <Serialization.hpp>
#include <TextAdventure.hpp>

class TextEngine {
public:
  struct Answer {
    std::string Text;
    bool Ends = false;
    ID ChoiceBoxIndex = -1;
    std::vector<std::pair<std::string, ID>> Choices;
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

  std::stack<ID> _choiceBoxes; // this is not included in State as it's empty
                               // when the game is being saved

  template <class T>
  static std::vector<T> removeFirstElement(const std::vector<T> &v) {
    if (v.size() < 1) {
      return {};
    }
    std::vector<T> a(v.size() - 1);
    std::copy(v.begin() + 1, v.end(), a.begin());
    return a;
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
    if (query.first == 0) { // _inv
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
    if (!found)
      return findAction({2, {0}});
    return res;
  }

  std::pair<ID, std::vector<std::string>>
  processQuery(const std::string &text) {
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
        if (!eCond.matches(_s.NounVals))
          viable = false;
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
      _s.NounVals.at(cmd.Arg1) = true;
      break;
    case Command::lose:
      _s.NounVals.at(cmd.Arg1) = false;
      break;
    case Command::go: {
      _s.NounVals.at(_s.Room) = false;
      _s.NounVals.at(cmd.Arg1) = true;
      _s.Room = cmd.Arg1;

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
      ans.Ends = ans.Ends || newAns.Ends;
      break;
    }
    case Command::choicebox:
      ans.ChoiceBoxIndex = cmd.Arg1;
      ans.Choices = doChoiceBox(cmd.Arg1);
      _choiceBoxes.push(ans.ChoiceBoxIndex);
      break;
    case Command::leave:
      // ignoring leave here, handled below
      break;
    case Command::end:
      ans.Ends = true;
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

    for (auto e : action.Commands)
      doCommand(e, ans);
    return ans;
  }

public:
  TextEngine() = default;
  TextEngine(const TextAdventure ta)
      : _ta(ta), _s{std::vector<uint16_t>(_ta.Actions.size(), 1),
                    std::vector<bool>(_ta.Nouns.size(), false), 1},
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
      query.first = 2; // _unk
      query.second = {};
    } else {
      query = {verbs.first, map<ID>(verbs.second, [this](auto a) -> ID {
                 ID res = 0;
                 for (auto &e : _ta.Nouns) {
                   for (auto &f : e.Aliases) {
                     if (a == f &&
                         (e.T != Noun::InventoryItem || _s.NounVals[res]) &&
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
        if (_choiceBoxes.size() < 2)
          ans.ChoiceBoxIndex = -1;
        else {
          _choiceBoxes.pop();
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
