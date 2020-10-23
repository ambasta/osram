#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <limits>
#include <vector>

#include <osram/util/coordinate_calculation.hxx>

namespace osram {
namespace util {
namespace coordinate_calculation {
double haversine_distance(const Coordinate first, const Coordinate second) {
  std::vector<int> i_coordinates{
      static_cast<int>(first.latitude), static_cast<int>(first.longitude),
      static_cast<int>(second.latitude), static_cast<int>(second.longitude)};
  std::vector<double> d_coordinates;

  std::for_each(i_coordinates.begin(), i_coordinates.end(),
                [&d_coordinates](const auto &coordinate) {
                  assert(coordinate != std::numeric_limits<int>::min());
                  d_coordinates.push_back(coordinate / COORDINATE_PRECISION *
                                          detail::DEGREE_TO_RADIANS);
                });

  const double delta_longitude = d_coordinates[3] - d_coordinates[1];
  const double delta_latitude = d_coordinates[2] - d_coordinates[0];

  const double aharv = std::pow(std::sin(delta_latitude / 2.0), 2.0) +
                       std::cos(d_coordinates[0]) * std::cos(d_coordinates[2]) *
                           std::pow(std::sin(delta_longitude / 2.), 2);
  const double charv =
      2. * std::atan2(std::sqrt(aharv), std::sqrt(1.0 - aharv));
  return detail::EARTH_RADIUS * charv;
}
} // namespace coordinate_calculation
} // namespace util
} // namespace osram
