#ifndef INCLUDED_Error_hpp
#define INCLUDED_Error_hpp

#include <iostream>
#include <cstdlib>

#define ERROR(E)                                                               \
  std::cout << E << std::endl;                                                 \
  std::exit(1)

#endif
