#ifndef OSRAM_UTIL_FINGERPRINT
#define OSRAM_UTIL_FINGERPRINT

#include <array>
#include <cstdint>
#include <type_traits>

namespace osram {
namespace util {
struct FingerPrint {
private:
  std::uint8_t calc_checksum() const;
  std::array<std::uint8_t, 4> magic_number;
  std::uint8_t major_version, minor_version, patch_version, checksum;

public:
  static FingerPrint get_valid();

  bool is_valid() const;

  bool is_data_compatible(const FingerPrint &) const;

  int get_major_version() const;

  int get_minor_version() const;

  int get_patch_version() const;
};

static_assert(sizeof(FingerPrint) == 8, "FingerPrint has unexpected size");
static_assert(std::is_trivial<FingerPrint>::value,
              "FingerPrint needs to be trivial");
static_assert(std::is_trivial<FingerPrint>::value, "FingerPrint needs to be a trivial");
} // namespace util
} // namespace osram
#endif
