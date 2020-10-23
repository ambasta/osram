#include <osram/util/coordinate.hxx>
#include <osram/util/coordinate_calculation.hxx>

namespace osram {
namespace util {
bool Coordinate::is_valid() const {
  return !(latitude > FixedLatitude{static_cast<std::int32_t>(
                          90 * COORDINATE_PRECISION)} or
           latitude < FixedLatitude{static_cast<std::int32_t>(
                          -90 * COORDINATE_PRECISION)} or
           longitude > FixedLongitude{static_cast<std::int32_t>(
                           180 * COORDINATE_PRECISION)} or
           longitude < FixedLongitude{static_cast<std::int32_t>(
                           -180 * COORDINATE_PRECISION)});
}

bool FloatCoordinate::is_valid() const {
  return !(latitude > FloatLatitude{90} or latitude < FloatLatitude{-90} or
           longitude > FloatLongitude{180} or longitude < FloatLongitude{-180});
}

bool operator==(const Coordinate lhs, const Coordinate rhs) {
  return lhs.latitude == rhs.latitude and lhs.longitude == rhs.longitude;
}

bool operator==(const FloatCoordinate lhs, const FloatCoordinate rhs) {
  return lhs.latitude == rhs.latitude and lhs.longitude == rhs.longitude;
}
} // namespace util
} // namespace osram
