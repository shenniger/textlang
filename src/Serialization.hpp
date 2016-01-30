#ifndef INCLUDED_Serialization_hpp
#define INCLUDED_Serialization_hpp

#include "Archive.hpp"

#define SERIALIZE(a)                                                           \
public:                                                                        \
  void serialize(Archive &ar) { ar a; }

#define SERIALIZE_EMPTY()                                                      \
public:                                                                        \
  void serialize(Archive &) {}

#endif
