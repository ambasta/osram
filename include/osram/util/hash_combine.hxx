#ifndef OSRAM_UTIL_HASH_COMBINE
#define OSRAM_UTIL_HASH_COMBINE

#include <functional>

namespace osram {
namespace util {
template <typename T> void hash_combine(std::size_t &seed, T const &value) {
  std::hash<T> hasher;
  seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
} // namespace util
} // namespace osram
#endif
