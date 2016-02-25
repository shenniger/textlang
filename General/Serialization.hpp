#ifndef INCLUDED_Serialization_hpp
#define INCLUDED_Serialization_hpp

#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

#define SERIALIZE(a)                                                           \
public:                                                                        \
  template <class Archive> void serialize(Archive &ar, const unsigned int) {   \
    ar a;                                                                      \
  }

#define SERIALIZE_EMPTY()                                                      \
public:                                                                        \
  template <class Archive> void serialize(Archive &, const unsigned int) {}

struct Serializer {
  template <class T> static void Read(T &t, std::istream &s) {
    boost::archive::binary_iarchive(s) >> t;
  }
  template <class T> static void Write(const T &t, std::ostream &s) {
    boost::archive::binary_oarchive(s) << t;
  }
};

#endif
