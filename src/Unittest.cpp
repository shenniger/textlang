#if (defined UNITTEST) || (defined COMPLETION)

#include "Unittest.hpp"
#include "CodeStream.hpp"
#include "Parser.hpp"

#define TEST_LIST                                                              \
  DEF_TEST(CodeStream)                                                         \
  DEF_TEST(Parser)

int main() {
  std::cout << "STARTING UNITTESTS" << std::endl;
  std::cout << "==================" << std::endl;

  TEST_LIST

  std::cout << "END OF UNITTESTS" << std::endl;

  return 0;
}

#endif
