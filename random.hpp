#pragma once

#include <limits>
#include <random>
#include <type_traits>

namespace ift630 {

template <typename RNG, typename T>
[[nodiscard]] auto random(RNG &&rng, const T lower_bound, const T upper_bound) noexcept -> T {
  static_assert(std::is_arithmetic<T>::value);
  static_assert(!std::is_same<T, bool>::value);

  auto dist = [lower_bound, upper_bound]() {
    if constexpr (std::is_floating_point<T>::value) {
      return std::uniform_real_distribution<T>{lower_bound, upper_bound};
    } else {
      return std::uniform_int_distribution<T>{lower_bound, upper_bound};
    }
  }();

  return dist(rng);
}

template <typename T>
[[nodiscard]] auto random(T lower_bound, T upper_bound) noexcept -> T {
  static std::random_device r{};
  return random(r, lower_bound, upper_bound);
}

template <typename T>
[[nodiscard]] auto random() noexcept -> T {
  if constexpr (std::is_floating_point<T>::value) {
    return random<T>(0.0, 1.0);
  } else if constexpr (std::is_same<T, bool>::value) {
    // uint16_t because the C++ standard says std::uniform_int_distribution on
    // 8-bit integer types is undefined behavior for some dumb reason
    return static_cast<bool>(random<uint16_t>(0, 1));
  } else {
    return random<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
  }
}

}  // namespace ift630
