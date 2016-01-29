#ifndef INCLUDED_CodeStream_hpp
#define INCLUDED_CodeStream_hpp

#include <string>
#include <vector>
#include "Unittest.hpp"
#include "Error.hpp"

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
      // TODO: Throw exception
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
};

#if (defined UNITTEST) || (defined COMPLETION)
DEF_UNITTEST(CodeStream) {
  const auto a = R"(
\ Test \
 Hello,	   World.
 			World, Hello.    Hello, World.

			     Test123

)";
  std::string res;
  for (CodeStream i(a); !i.eof();) {
    res += i.next();
  }
  std::cout << "Result: " << res << std::endl;
  assert(res == " Hello, World. World, Hello. Hello, World. Test123 ");
}
END_UNITTEST
#endif

#endif
