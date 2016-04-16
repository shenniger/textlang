#ifndef INCLUDED_Error_hpp
#define INCLUDED_Error_hpp

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <textlang/Internal/Algorithms.hpp>

extern std::string curFile; // from Starter.cpp

#define ERROR(E)                                                               \
  std::cout << curFile << ":" << E << std::endl;                               \
  std::exit(1)

#endif
