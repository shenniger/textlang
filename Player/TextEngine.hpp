#ifndef INCLUDED_TextEngine_hpp
#define INCLUDED_TextEngine_hpp

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <regex>
#include <utility>
#include <Algorithms.hpp>
#include <Error.hpp>
#include <TextAdventure.hpp>

class TextEngine {
public:
  struct Answer {
    std::string Text;
    bool Ends = false;
    ID ChoiceBoxIndex = -1;
    std::vector<std::pair<std::string, ID>> Choices;
  };

private:
  TextAdventure _ta;
  std::vector<std::vector<std::regex>> _verbs;
  std::vector<uint16_t> _actI;
  std::vector<bool> _nounVals;
  ID Room;

  template <class T>
  static std::vector<T> removeFirstElement(const std::vector<T> &v) {
    if (v.size() < 1) {
      return {};
    }
    std::vector<T> a(v.size() - 1);
    std::copy(v.begin() + 1, v.end(), a.begin());
    return a;
  }

  Action findAction(const std::pair<ID, std::vector<ID>> &query) const {
    Action res;
    uint16_t greatestVal = 0;
    size_t i = 0;
    bool found = false;
    for (const auto &e : _ta.Actions) {
      if (e.Selector.matches(query.first, query.second, _nounVals) &&
          _actI.at(i) >= e.Selector.I && e.Selector.I > greatestVal) {
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
        if (!eCond.matches(_nounVals))
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
      _nounVals.at(cmd.Arg1) = true;
      break;
    case Command::lose:
      _nounVals.at(cmd.Arg1) = false;
      break;
    case Command::go:
      _nounVals[Room] = false;
      _nounVals[cmd.Arg1] = true;
      Room = cmd.Arg1;

      for (const auto &e : _ta.Actions) {
        if (e.Selector.Verb == 1 && e.Selector.Nouns[0] == Room) {
          // found
          ans.Text += "\n";
          ans.Text += e.Result;
          for (auto ele : e.Commands)
            doCommand(ele, ans);
        }
      }
      break;
    case Command::choicebox:
      ans.ChoiceBoxIndex = cmd.Arg1;
      ans.Choices = doChoiceBox(cmd.Arg1);
      break;
    case Command::leave:
      // ignoring leave here, handled below
      break;
    case Command::end:
      ans.Ends = true;
      break;
    }
  }

public:
  TextEngine() = default;
  TextEngine(const TextAdventure ta)
      : _ta(ta), _verbs(map<std::vector<std::regex>>(
                     _ta.Verbs,
                     [](Verb a) {
                       return map<std::regex>(a.Regexes, [](auto a) {
                         return std::regex(a, std::regex_constants::icase);
                       });
                     })),
        _actI(_ta.Actions.size(), 1), _nounVals(_ta.Nouns.size(), false),
        Room(1) {}

  Answer begin() {
    _nounVals.at(1) = true;
    Answer ans;
    for (const auto &e : _ta.Actions) {
      if (e.Selector.Verb == 1 && e.Selector.Nouns[0] == Room) {
        // found
        ans.Text += "\n";
        ans.Text += e.Result;
        for (auto ele : e.Commands)
          doCommand(ele, ans);
      }
    }
    return ans;
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
                     if (f == a &&
                         (e.T != Noun::InventoryItem || _nounVals[res]) &&
                         (e.InRoom == -1 || e.InRoom == Room))
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

    // find action
    auto action = findAction(query);

    Answer ans;
    ans.Text = action.Result;

    // incrementing I
    auto itActI = _actI.begin();
    auto itActions = _ta.Actions.begin();
    while (itActI != _actI.end()) {
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

  Answer choiceBoxQuery(const ID ChoiceBox, const ID Option) {
    auto entry = _ta.ChoiceBoxes.at(ChoiceBox).C.at(Option);
    Answer ans;
    ans.ChoiceBoxIndex = ChoiceBox;
    for (const auto &e : entry.Commands) {
      if (e.T == Command::leave) {
        ans.ChoiceBoxIndex = -1;
      }
      doCommand(e, ans);
    }
    ans.Text = entry.Text;
    if (ans.ChoiceBoxIndex != -1) {
      ans.Choices = doChoiceBox(ans.ChoiceBoxIndex);
    }
    return ans;
  }
};

#endif
