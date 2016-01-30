#ifndef INCLUDED_Archive_hpp
#define INCLUDED_Archive_hpp

class Archive;

#include <stddef.h>
#include <stdint.h>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

class Archive {
private:
  // is in serialization mode? else, in deserialization mode
  bool serialize = 0;
  std::ostream *os;
  std::istream *is;

  template <class T, std::enable_if_t<std::is_enum<T>{} ||
                                      std::is_arithmetic<T>{}> * = nullptr>
  void in(T &a) {
    is->read((char *)&a, sizeof(T));
  }
  template <class T, std::enable_if_t<std::is_enum<T>{} ||
                                      std::is_arithmetic<T>{}> * = nullptr>
  void out(T a) {
    os->write((char *)&a, sizeof(T));
  }

  void in(std::string &s) {
    s = std::string();
    size_t num;
    in(num);
    s.reserve(num);
    for (size_t i = 0; i < num; i++) {
      char c;
      in(c);
      s += c;
    }
  }
  void out(std::string &s) {
    out(s.length());
    for (auto e : s) {
      out(e);
    }
  }

  template <class T> void in(std::vector<T> &a) {
    a = std::vector<T>();
    size_t num;
    in(num);
    a.reserve(num);
    for (size_t i = 0; i < num; i++) {
      T t;
      in(t);
      a.push_back(t);
    }
  }
  template <class T> void out(std::vector<T> &a) {
    out(a.size());
    for (auto &e : a) {
      out(e);
    }
  }

  template <class T1, class T2> void in(std::pair<T1, T2> &a) {
    in(a.first);
    in(a.second);
  }
  template <class T1, class T2> void out(std::pair<T1, T2> &a) {
    out(a.first);
    out(a.second);
  }

  template <class T> void in(std::shared_ptr<T> &a) {
    T *ptr;
    in(ptr);
    a = std::shared_ptr<T>(ptr);
  }
  template <class T> void out(std::shared_ptr<T> &a) { out(a.get()); }

  template <class T, std::enable_if_t<(!std::is_enum<T>{}) &&
                                      (!std::is_arithmetic<T>{})> * = nullptr>
  void in(T &a) {
    a.serialize(*this);
  }
  template <class T, std::enable_if_t<(!std::is_enum<T>{}) &&
                                      (!std::is_arithmetic<T>{})> * = nullptr>
  void out(T &a) {
    a.serialize(*this);
  }

public:
  Archive(std::ostream *o) {
    serialize = true;
    os = o;
  }
  Archive(std::istream *i) {
    serialize = false;
    is = i;
  }
  template <class T> Archive &operator&(T &a) {
    if (serialize) {
      out(a);
    } else {
      in(a);
    }
    return *this;
  }
};

#endif
