#ifndef OSRAM_UTIL_GUIDANCE_ENTRY_CLASS
#define OSRAM_UTIL_GUIDANCE_ENTRY_CLASS
#include <cstdint>
#include <type_traits>
namespace osram {
namespace util {
namespace guidance {
class EntryClass {
  using FlagBaseType = std::uint32_t;

private:
  FlagBaseType enabled_entries_flags;

public:
  constexpr EntryClass() : enabled_entries_flags(0) {}

  bool activate(std::uint32_t index);

  bool allows_entry(std::uint32_t index) const;

  bool operator==(const EntryClass &) const;

  bool operator<(const EntryClass &) const;
};

static_assert(std::is_trivially_copyable<EntryClass>::value,
              "Class is serialized trivially in "
              "the datafacades. Bytewise writing "
              "requires trivially copyable type");
} // namespace guidance
} // namespace util

constexpr const util::guidance::EntryClass EMPTY_ENTRY_CLASS{};
} // namespace osram
#endif
