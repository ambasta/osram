#ifndef OSRAM_UTIL_COORDINATE
#define OSRAM_UTIL_COORDINATE

#include <cmath>
#include <cstdint>
#include <limits>
#include <osram/util/alias.hxx>
#include <string>
#include <type_traits>

namespace osram {
constexpr const double COORDINATE_PRECISION = 1e6;

namespace util {
using FixedLatitude = std::int32_t;
using FixedLongitude = std::int32_t;
using FloatLatitude = double;
using FloatLongitude = double;

inline std::int32_t to_fixed(const double floating) {
  const auto fixed =
      static_cast<std::int32_t>(std::round(floating * COORDINATE_PRECISION));
  return std::int32_t(fixed);
}

inline double to_floating(const std::int32_t fixed) {
  const auto floating = static_cast<double>(fixed) / COORDINATE_PRECISION;
  return double{floating};
}

struct FloatCoordinate;

struct Coordinate {
  std::int32_t latitude, longitude;

  Coordinate()
      : longitude{std::numeric_limits<std::int32_t>::min()},
        latitude{std::numeric_limits<std::int32_t>::min()} {}

  Coordinate(const FloatCoordinate &);

  Coordinate(const double longitude, const double latitude)
      : Coordinate(to_fixed(longitude), to_fixed(latitude)) {}

  Coordinate(const std::int32_t longitude, const std::int32_t latitude)
      : longitude(longitude), latitude(latitude) {}

  template <class T>
  Coordinate(const T &coordinate)
      : longitude(coordinate.longitude), latitude(coordinate.latitude) {
    static_assert(!std::is_same<T, Coordinate>::value,
                  "This constructor should not be used for Coordinates");
    static_assert(std::is_same<decltype(longitude),
                               decltype(coordinate.longitude)>::value,
                  "Coordinate types incompatible");
    static_assert(
        std::is_same<decltype(latitude), decltype(coordinate.latitude)>::value,
        "Coordinate types incompatible");
  }

  bool is_valid() const;
  friend bool operator==(const Coordinate, const Coordinate);
};

struct FloatCoordinate {
  double latitude, longitude;

  FloatCoordinate()
      : longitude{std::numeric_limits<double>::min()},
        latitude{std::numeric_limits<double>::min()} {}

  FloatCoordinate(const Coordinate other)
      : FloatCoordinate(to_floating(other.longitude),
                        to_floating(other.latitude)) {}

  FloatCoordinate(const double longitude, const double latitude)
      : longitude(longitude), latitude(latitude) {}

  bool is_valid() const;

  friend bool operator==(const FloatCoordinate, const FloatCoordinate);
};

bool operator==(const Coordinate, const Coordinate);
bool operator==(const FloatCoordinate, const FloatCoordinate);

inline Coordinate::Coordinate(const FloatCoordinate &other)
    : Coordinate(to_fixed(other.longitude), to_fixed(other.latitude)) {}
} // namespace util
} // namespace osram
#endif
