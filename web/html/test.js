function escapeSingle(str) {
  var chars = {"&": "&amp;", "<": "&lt;", ">": "&gt;",
               '"': '&quot;', "'": '&#39;', "/": '&#x2F;' };
  return String(str).replace(/[&<>"'\/]/g, function (v) {
    return chars[v];
  });
}

function prepareString(str) {
  return str.split("\n")
            .map(function (s) { return escapeSingle(s); })
            .reduce(function (p, c) { p += c + "<br />"; return p; }, String(""));
}

var curTA;

function genCommandVisual(str) {
  var ele = document.createElement("div");
  ele.classList.add("cmd");
  ele.innerHTML = escapeSingle(str);
  return ele;
}
function genAnswerVisual(str) {
  var ele = document.createElement("div");
  ele.classList.add("response");
  ele.innerHTML = prepareString(str);
  return ele;
}
function genChoice(id, text) {
  var ele = document.createElement("a");
  ele.classList.add("choice");
  ele.setAttribute("href", "javascript:void(0);");
  ele.setAttribute("data-choiceID", id);
  ele.addEventListener("click", function (event) {
    sendChoiceBox(event);
  });
  ele.innerHTML = escapeSingle(text);
  return ele;
}
function genChoiceBox(choiceBoxID, choices) {
  var ele = document.createElement("div");
  ele.classList.add("choiceBox");
  ele.setAttribute("data-id", choiceBoxID);
  for(i = 0; i < choices.length; i++) {
    ele.appendChild(genChoice(choices[i].id, choices[i].text));
  }
  return ele;
}

function interpretAnswer(ans) {
  if(ans.text != "") {
    document.getElementsByClassName("texts")[0].appendChild(genAnswerVisual(ans.text)).scrollIntoView();
  }
  document.getElementsByClassName("choiceBoxContainer")[0].innerHTML = "";
  if(ans.choiceBoxIndex != -1) {
    document.getElementsByClassName("command")[0].style = "display:none;";
    document.getElementsByClassName("choiceBoxContainer")[0].appendChild(genChoiceBox(ans.choiceBoxIndex, ans.choiceBox));
    document.getElementsByClassName("choiceBoxContainer")[0].scrollIntoView();
  }
  else {
    document.getElementsByClassName("command")[0].style = "display:block;";
  }
}

function sendChoiceBox(event) {
  var choiceID = parseInt(event.target.getAttribute("data-choiceID"));
  var boxID = parseInt(document.getElementsByClassName("choiceBox")[0].getAttribute("data-id"));
  interpretAnswer(choiceBoxQueryTextAdventure(curTA, boxID, choiceID));
}

function sendCommand(str) {
  document.getElementsByClassName("texts")[0].appendChild(genCommandVisual(str));
  interpretAnswer(queryTextAdventure(curTA, str));
}

function init() {
  document.getElementsByClassName("cmd_input")[0].
           addEventListener("keydown", function (event) {
    if(event.keyCode === 13) {
      sendCommand(document.getElementsByClassName("cmd_input")[0].value);
      document.getElementsByClassName("cmd_input")[0].value = "";
    }
  });

  loadA("test", function(a) {
    curTA = a;
    console.log("got: " + a);
    interpretAnswer(beginTextAdventure(curTA));
  });
}
