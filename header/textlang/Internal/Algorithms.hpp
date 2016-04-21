#ifndef INCLUDED_Algorithms_hpp
#define INCLUDED_Algorithms_hpp

#include <stddef.h>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <limits>
#include <algorithm>

const auto notFound = std::numeric_limits<size_t>::max();

template <class T>
size_t find(const std::vector<T> &a, const T &t) {
  size_t res = 0;
  for (const auto &e : a) {
    if (e == t) return res;
    res++;
  }
  return notFound;
}

template <class TOut, class TInContainer, class TFunc>
std::vector<TOut> map(TInContainer in, TFunc func) {
  std::vector<TOut> res;
  res.resize(in.size());
  std::transform(in.begin(), in.end(), res.begin(), func);
  return res;
}

template <class T, class Func>
size_t find(const std::vector<T> &a, Func f) {
  size_t res = 0;
  for (const auto &e : a) {
    if (f(e)) return res;
    res++;
  }
  return notFound;
}

#endif
