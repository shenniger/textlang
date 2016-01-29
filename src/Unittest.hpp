#if (defined UNITTEST) || (defined COMPLETION)

#include <iostream>
#include <exception>
#include <cassert>

#define DEF_TEST(f)                                                            \
  try {                                                                        \
    UNITTEST_##f<void>();                                                      \
  } catch (std::exception & e) {                                               \
    std::cout << "Exception occured in " << __STRING(f) << ": " << e.what()    \
              << std::endl;                                                    \
  } catch (...) {                                                              \
    std::cout << "Exception occured in " << __STRING(f)                        \
              << ": Unhandled exception." << std::endl;                        \
  }

#define DEF_UNITTEST(f)                                                        \
  template <class T> void UNITTEST_##f() {                                     \
    std::cout << "Started Unittest " << __STRING(f) << std::endl;

#define END_UNITTEST                                                           \
  std::cout << "Ended Unittest" << std::endl;                                  \
  }

#endif
