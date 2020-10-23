#include <cassert>
#include <climits>
#include <osram/util/guidance/entry_class.hxx>

namespace osram {
namespace util {
namespace guidance {

bool EntryClass::activate(std::uint32_t index) {
  if (index >= CHAR_BIT * sizeof(FlagBaseType))
    return false;
  enabled_entries_flags |= (FlagBaseType{1} << index);
  return true;
}

bool EntryClass::allows_entry(std::uint32_t index) const {
  assert(index < CHAR_BIT * sizeof(FlagBaseType));
  return 0 != (enabled_entries_flags & (FlagBaseType{1} << index));
}

bool EntryClass::operator==(const EntryClass &other) const {
  return enabled_entries_flags == other.enabled_entries_flags;
}

bool EntryClass::operator<(const EntryClass &other) const {
  return enabled_entries_flags < other.enabled_entries_flags;
}

} // namespace guidance
} // namespace util
} // namespace osram
