#ifndef OSRAM_UTIL_COORDINATE_CALCULATION
#define OSRAM_UTIL_COORDINATE_CALCULATION
#include <numbers>
#include <osram/util/coordinate.hxx>

namespace osram {
namespace util {
namespace coordinate_calculation {
namespace detail {
const constexpr double DEGREE_TO_RADIANS = 0.017453292519943295769236907684886;
const constexpr double RADIANS_TO_DEGREE = 1. / DEGREE_TO_RADIANS;
const constexpr long double EARTH_RADIUS = 6372797.560856;

inline double degree_to_radians(const double degree) {
  return degree * (std::numbers::pi / 180.0);
}

inline double radians_to_degree(const double radian) {
  return radian * (180.0 * (1. / std::numbers::pi));
}
} // namespace detail
double haversine_distance(const Coordinate, const Coordinate);

std::uint64_t squared_euclidean_distance(const Coordinate, const Coordinate);

double perpendicular_distance(const Coordinate, const Coordinate,
                              const Coordinate);

double perpendicular_distance(const Coordinate, const Coordinate,
                              const Coordinate, Coordinate &, double &);

double fcc_approximate_distance(const Coordinate, const Coordinate);

double bearing(const Coordinate, const Coordinate);

bool is_ccw(const Coordinate, const Coordinate, const Coordinate);

Coordinate centroid(const Coordinate, const Coordinate);

inline std::pair<double, FloatCoordinate>
project_point_on_segment(const FloatCoordinate &, const FloatCoordinate &,
                         const FloatCoordinate &);
} // namespace coordinate_calculation
} // namespace util
} // namespace osram
#endif
