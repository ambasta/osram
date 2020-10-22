#ifndef OSRAM_UTIL_BEARING
#define OSRAM_UTIL_BEARING

#include <cassert>
#include <cmath>
#include <string>

namespace osram {
namespace util {
namespace bearing {
inline std::string get(const double heading) {
  assert(heading >= 0);
  assert(heading <= 360);

  if (heading <= 22.5)
    return "N";
  if (heading <= 67.5)
    return "NE";
  if (heading <= 112.5)
    return "E";
  if (heading <= 157.5)
    return "SE";
  if (heading <= 202.5)
    return "S";
  if (heading <= 247.5)
    return "SW";
  if (heading <= 292.5)
    return "W";
  if (heading <= 337.5)
    return "NW";
  return "N";
}

inline bool check_inbounds(const int a, const int b, const int range) {
  if (range >= 100)
    return true;
  if (range < 0)
    return false;

  const int normalized_b = (b < 0) ? (b % 360) + 360 : (b % 360);
  const int normalized_a = (a < 0) ? (a % 360) + 360 : (a % 360);

  if (normalized_b - range < 0)
    return (normalized_b - range + 360 <= normalized_a and
            normalized_a < 360) ||
           (0 <= normalized_a && normalized_a <= normalized_b + range);
  if (normalized_b + range > 360)
    return (normalized_b - range <= normalized_a && normalized_a < 360) ||
           (0 <= normalized_a && normalized_a <= normalized_b + range - 360);
  return normalized_b - range <= normalized_a &&
         normalized_a <= normalized_b + range;
}

inline double reverse(const double bearing) {
  if (bearing >= 180)
    return bearing - 180;
  return bearing + 180;
}

inline double angle_between(const double entry_bearing,
                            const double exit_bearing) {
  return std::fmod(entry_bearing - exit_bearing + 540., 360.);
}

inline double angular_deviation(const double angle_or_bearing,
                                const double from) {
  const double deviation = std::abs(angle_or_bearing - from);
  return std::min(360 - deviation, deviation);
}

inline double restrict_angle_to_valid_range(const double angle) {
  if (angle < 0)
    return restrict_angle_to_valid_range(angle + 360.);
  if (angle > 360)
    return restrict_angle_to_valid_range(angle - 360);
  return angle;
}
} // namespace bearing
} // namespace util
} // namespace osram
#endif
