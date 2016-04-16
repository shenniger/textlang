function _answer(hAns) { // this TAKES OWNERSHIP of hAns
  var res = new Object();
  res.text = Module.Pointer_stringify(Module._answerText(hAns));
  res.choiceBoxIndex = Module._answerChoiceBoxIndex(hAns);
  if(res.choiceBoxIndex !== -1) {
    var len = Module._answerChoiceBoxSize(hAns);
    res.choiceBox = new Array(len);
    for(i = 0; i < len; i++) {
      var choice = new Object();
      choice.text = Module.Pointer_stringify(Module._answerChoiceBoxEntry(hAns, i));
      choice.id = Module._answerChoiceBoxNum(hAns, i);
      res.choiceBox[i] = choice;
    }
  }
  else {
    res.choiceBox = new Array();
  }
  res.clientAction = Module._answerClientAction(hAns);
  Module._destroyAnswer(hAns);
  return res;
}

function loadA(str, func) {
  __ta_last_func = func;
  var cstr = allocate(intArrayFromString(str), 'i8', ALLOC_NORMAL);
  Module._loadTextAdventure(cstr);
  Module._free(cstr);
}

function beginTextAdventure(adv) {
  return _answer(Module._beginTextAdventure(adv));
}

function choiceBoxQueryTextAdventure(adv, choiceBoxID, choiceID) {
  return _answer(Module._choiceBoxQueryTextAdventure(adv, choiceBoxID, choiceID));
}

function queryTextAdventure(adv, str) {
  var cstr = allocate(intArrayFromString(str), 'i8', ALLOC_NORMAL);
  var hAns = Module._queryTextAdventure(adv, cstr);
  Module._free(cstr);
  return _answer(hAns);
}
