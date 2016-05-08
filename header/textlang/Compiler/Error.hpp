#ifndef INCLUDED_Error_hpp
#define INCLUDED_Error_hpp

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <textlang/Internal/Algorithms.hpp>

extern std::string curFile;  // from Starter.cpp

#define ERROR(E)                                              \
  std::cout << "ERROR: " << curFile << ":" << E << std::endl; \
  std::exit(1)

#define ERROR_GEN(E)                        \
  std::cout << "ERROR: " << E << std::endl; \
  std::exit(1)

#define WARN(E) std::cout << "WARN: " << curFile << ":" << E << std::endl;
#define WARN_GEN(E) std::cout << "WARN: " << E << std::endl;

#endif
