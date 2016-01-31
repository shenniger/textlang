#ifndef INCLUDED_CodeStream_hpp
#define INCLUDED_CodeStream_hpp

#include <string>
#include <vector>
#include <Error.hpp>

class CodeStream {
  std::string _s;
  std::string::iterator _it;
  bool _inWhitespace = false;
  bool _inComment = false;

  bool isWhitespace(char a) { return a == ' ' || a == 9 || a == 13 || a == 10; }

public:
  char last;

  CodeStream(const std::string &s) : _s(s) { _it = _s.begin(); }

  char rawRead() {
    if (_it == _s.end()) {
      ERROR("Unexpected EOF!");
    }
    return *_it++;
  }

  bool eof() {
    if (_inWhitespace) {
      for (auto i = _it; i != _s.end(); i++) {
        if (!isWhitespace(*i)) {
          return false;
        }
      }
      return true;
    }
    return _it == _s.end();
  }

  char next() {
    char a = rawRead();
    if (a == '\\') {
      _inComment = !_inComment;
      return next();
    }
    if (_inComment) {
      return next();
    }
    if (isWhitespace(a)) {
      if (_inWhitespace)
        return next();
      else {
        _inWhitespace = true;
        return last = ' ';
      }
    } else {
      _inWhitespace = false;
      return last = a;
    }
  }

  void rewind() { _it--; }
};

#endif
