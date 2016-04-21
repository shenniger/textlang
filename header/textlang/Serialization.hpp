#ifndef INCLUDED_Serialization_hpp
#define INCLUDED_Serialization_hpp

#ifdef USE_BOOST_SERIALIZATION

#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

#define SERIALIZE(a)                                \
 public:                                            \
  template <class Archive>                          \
  void serialize(Archive &ar, const unsigned int) { \
    ar a;                                           \
  }

#define SERIALIZE_EMPTY()  \
 public:                   \
  template <class Archive> \
  void serialize(Archive &, const unsigned int) {}

struct Serializer {
  template <class T>
  static void Read(T &t, std::istream &s) {
    boost::archive::binary_iarchive(s) >> t;
  }
  template <class T>
  static void Write(const T &t, std::ostream &s) {
    boost::archive::binary_oarchive(s) << t;
  }
};

#elif USE_OWN_SERIALIZATION

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>

struct Archive {
  std::istream *is;
  std::ostream *os;

  using LengthType = uint32_t;

  template <class T, std::enable_if_t<std::is_enum<T>{} ||
                                      std::is_arithmetic<T>{}> * = nullptr>
  void in(T &a) {
    is->read(reinterpret_cast<char *>(&a), sizeof(T));
  }
  template <class T, std::enable_if_t<std::is_enum<T>{} ||
                                      std::is_arithmetic<T>{}> * = nullptr>
  void out(T a) {
    os->write(reinterpret_cast<char *>(&a), sizeof(T));
  }

  template <class T>
  void in(std::basic_string<T> &s) {
    static_assert(
        std::is_arithmetic<T>{},
        "Serialization expects arithmetic types for std::basic_string");

    LengthType num;
    in(num);
    s = std::basic_string<T>(num, static_cast<T>(0));
    is->read(reinterpret_cast<char *>(&(s[0])), num * sizeof(T));
  }
  template <class T>
  void out(std::basic_string<T> &s) {
    static_assert(
        std::is_arithmetic<T>{},
        "Serialization expects arithmetic types for std::basic_string");

    LengthType num = s.size();
    out(num);
    os->write(reinterpret_cast<char *>(&(s[0])), num * sizeof(T));
  }

  template <class T>
  void in(std::vector<T> &a) {
    a = std::vector<T>();
    LengthType num;
    in(num);
    a.reserve(num);
    for (size_t i = 0; i < num; i++) {
      T t;
      in(t);
      a.push_back(t);
    }
  }
  template <class T>
  void out(std::vector<T> &a) {
    LengthType num = a.size();
    out(num);
    for (auto &e : a) {
      out(e);
    }
  }

  void in(std::vector<bool> &a) {
    a = std::vector<bool>();
    LengthType num;
    in(num);
    a.reserve(num);
    for (size_t i = 0; i < num; i++) {
      bool t;
      in(t);
      a.push_back(t);
    }
  }
  void out(std::vector<bool> &a) {
    LengthType num = a.size();
    out(num);
    for (auto e : a) {
      out(static_cast<bool>(e));
    }
  }

  template <class T1, class T2>
  void in(std::pair<T1, T2> &a) {
    in(a.first);
    in(a.second);
  }
  template <class T1, class T2>
  void out(std::pair<T1, T2> &a) {
    out(a.first);
    out(a.second);
  }

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
  Archive(std::istream &i) : is(&i), os(nullptr) {}
  Archive(std::ostream &o) : is(nullptr), os(&o) {}

  template <class T>
  Archive &operator&(T &val) {
    if (is != nullptr) {
      in(val);
    } else if (os != nullptr) {
      out(val);
    }
    return *this;
  }
};

#define SERIALIZE(a) \
 public:             \
  void serialize(Archive &ar) { ar a; }

#define SERIALIZE_EMPTY() \
 public:                  \
  void serialize(Archive &) {}

struct Serializer {
  template <class T>
  static void Read(T &t, std::istream &s) {
    Archive(s) & t;
  }
  template <class T>
  static void Write(T &t, std::ostream &s) {
    Archive(s) & t;
  }
};

#else
#error Please specify serialization method. \
Either use -DUSE_BOOST_SERIALIZATION or use -DUSE_OWN_SERIALIZATION.
#endif

#endif
