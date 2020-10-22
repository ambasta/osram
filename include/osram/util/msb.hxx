#ifndef OSRAM_UTILS_MSB
#define OSRAM_UTILS_MSB

#include <type_traits>
#include <cassert>

template <typename T> std::size_t msb(T value) {
  static_assert(std::is_integral<T>::value, "Only integral types supported");
  std::size_t msb = 0;

  while (value > 0) {
    value >>= 1u;
    msb++;
  }

  assert(msb > 0);
  return msb - 1;
}

#endif
