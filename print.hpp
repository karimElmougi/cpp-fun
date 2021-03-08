#pragma once

#include <iostream>
#include <sstream>

namespace ift630 {

template <typename T>
void println(T&& t) {
  auto s = std::stringstream{};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay)
  s << t << '\n';  // it is fine to let char[] decay to char* here
  std::cout << s.str();
}

inline void println() { std::cout << '\n'; }

}  // namespace ift630
