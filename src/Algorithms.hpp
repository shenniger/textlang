#ifndef INCLUDED_Algorithms_hpp
#define INCLUDED_Algorithms_hpp

#include <stddef.h>
#include <string>
#include <vector>
#include <limits>

const auto notFound = std::numeric_limits<size_t>::max();

template <class T> size_t find(const std::vector<T> &a, T t) {
  size_t res = 0;
  for (const auto &e : a) {
    if (e == t)
      return res;
    res++;
  }
  return -1;
}

template <class T, class Func> size_t find(const std::vector<T> &a, Func f) {
  size_t res = 0;
  for (const auto &e : a) {
    if (f(e))
      return res;
    res++;
  }
  return -1;
}

#endif
