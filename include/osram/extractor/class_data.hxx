#ifndef OSRAM_EXTRACTOR_CLASS_DATA
#define OSRAM_EXTRACTOR_CLASS_DATA

#include <osram/util/bit_range.hxx>

namespace osram {
namespace extractor {
using ClassData = std::uint8_t;

constexpr ClassData INVALID_CLASS_DATA = std::numeric_limits<ClassData>::max();

static const std::uint8_t MAX_CLASS_INDEX = 8 - 1;
static const std::uint8_t MAX_EXCLUDABLE_CLASSES = 8;

inline bool isSubset(const ClassData lhs, const ClassData rhs) {
  return (lhs & rhs) == lhs;
}

inline auto getClassIndexes(const ClassData data) {
  return osram::util::make_bit_range<ClassData>(data);
}

inline auto getClassData(const std::size_t index) {
  assert(index <= MAX_CLASS_INDEX);
  return uint8_t{1} << index;
}

inline bool is_valid_class_name(const std::string &name) {
  return std::find_if_not(name.cbegin(), name.cend(), [](const auto ch) {
           return std::isalnum(ch);
         }) == name.end();
}
} // namespace extractor
} // namespace osram
#endif
