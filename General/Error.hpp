#ifndef INCLUDED_Error_hpp
#define INCLUDED_Error_hpp

#include <cstdlib>
#include <iostream>
#include <string>

extern std::string curFile; // from Starter.cpp

#define ERROR(E)                                                               \
  std::cout << curFile << ":" << E << std::endl;                               \
  std::exit(1)

#endif
