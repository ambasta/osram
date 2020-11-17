#ifndef OSRAM_EXTRACTOR_CLASS_DATA
#define OSRAM_EXTRACTOR_CLASS_DATA
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <limits>
#include <ranges>
#include <string_view>

namespace osram {
namespace extractor {
using ClassData = std::uint8_t;
constexpr ClassData INVALID_CLASS_DATA = std::numeric_limits<ClassData>::max();
static const std::uint8_t MAX_CLASS_INDEX = 8 - 1;
static const std::uint8_t MAX_EXCLUDABLE_CLASSES = 8;

inline bool is_subset(const ClassData lhs, const ClassData rhs) {
  // TODO originally, lhs & rhs == lhs
  // move to non bitwise values
  return false;
}

inline auto get_class_indices(const ClassData data) {
  return std::ranges::iota_view(1, (int)data);
}

inline auto get_class_data(const std::size_t index) {
  assert(index < MAX_CLASS_INDEX);
  return std::pow(2, index);
}

inline bool is_valid_class_name(std::string_view name) {
  return std::find_if_not(name.begin(), name.end(), [](const auto chr) {
           return std::isalnum(chr);
         }) == name.end();
}
} // namespace extractor
} // namespace osram
#endif
