#include <bits/stdint-intn.h>
#include <cassert>
#include <cstdint>
#include <limits>

#include <osram/util/coordinate.hxx>
#include <osram/util/rectangle.hxx>

namespace osram {
namespace util {
RectangleInt2D::RectangleInt2D()
    : min_lon{std::numeric_limits<std::int32_t>::max()},
      max_lon{std::numeric_limits<std::int32_t>::min()},
      min_lat{std::numeric_limits<std::int32_t>::max()},
      max_lat{std::numeric_limits<std::int32_t>::min()} {}

RectangleInt2D::RectangleInt2D(FixedLongitude min_lon, FixedLongitude max_lon,
                               FixedLatitude min_lat, FixedLatitude max_lat)
    : min_lon(min_lon), max_lon(max_lon), min_lat(min_lat), max_lat(max_lat) {}

RectangleInt2D::RectangleInt2D(FloatLongitude min_lon, FloatLongitude max_lon,
                               FloatLatitude min_lat, FloatLatitude max_lat)
    : min_lon(to_fixed(min_lon)), max_lon(to_fixed(max_lon)),
      min_lat(to_fixed(min_lat)), max_lat(to_fixed(max_lat)) {}

void RectangleInt2D::merge_bounding_boxes(const RectangleInt2D &other) {
  min_lon = std::min(min_lon, other.min_lon);
  max_lon = std::max(max_lon, other.max_lon);
  min_lat = std::min(min_lat, other.min_lat);
  max_lat = std::max(max_lat, other.max_lat);
  assert(min_lon != FixedLongitude{std::numeric_limits<std::int32_t>::min()});
  assert(min_lat != FixedLatitude{std::numeric_limits<std::int32_t>::min()});
  assert(max_lon != FixedLongitude{std::numeric_limits<std::int32_t>::min()});
  assert(max_lat != FixedLatitude{std::numeric_limits<std::int32_t>::min()});
}

Coordinate RectangleInt2D::centroid() const {
  Coordinate cent;
  cent.longitude = (min_lon + max_lon) / FixedLongitude{2};
  cent.latitude = (min_lat + max_lat) / FixedLatitude{2};
  return cent;
}

bool RectangleInt2D::intersects(const RectangleInt2D &other) const {
  return !(max_lon < other.min_lon or min_lon > other.max_lon or
           max_lat < other.min_lat or min_lat > other.max_lat);
}

std::uint64_t
RectangleInt2D::get_min_squared_dist(const Coordinate coordinate) const {
  const bool is_contained = contains(coordinate);
  if (is_contained)
    return 0.0f;

  enum Direction {
    INVALID = 0,
    NORTH = 1,
    SOUTH = 2,
    EAST = 4,
    NORTH_EAST = 5,
    SOUTH_EAST = 6,
    WEST = 8,
    NORTH_WEST = 9,
    SOUTH_WEST = 10
  };

  Direction direction = INVALID;

  if (coordinate.latitude > max_lat)
    direction = (Direction)(direction | NORTH);
  else if (coordinate.latitude < min_lat)
    direction = (Direction)(direction | SOUTH);
  else if (coordinate.longitude > max_lon)
    direction = (Direction)(direction | EAST);
  else if (coordinate.longitude < min_lon)
    direction = (Direction)(direction | WEST);

  assert(direction != INVALID);

  std::uint64_t min_distance = std::numeric_limits<std::uint64_t>::max();
  Coordinate other{max_lon, max_lat};

  switch (direction) {
  case SOUTH:
    other.latitude = min_lat;
  case NORTH:
    other.longitude = coordinate.longitude;
    break;
  case WEST:
    other.longitude = min_lon;
  case EAST:
    other.latitude = coordinate.latitude;
    break;
  case NORTH_WEST:
    other.longitude = min_lon;
    break;
  case SOUTH_EAST:
    other.latitude = min_lat;
    break;
  case SOUTH_WEST:
    other.longitude = min_lon;
    other.latitude = min_lat;
    break;
  default:
    break;
  }

  min_distance =
      coordinate_calculation::squared_euclidean_distance(coordinate, other);
  assert(min_distance < std::numeric_limits<std::uint64_t>::max());
  return min_distance;
}

bool RectangleInt2D::contains(const Coordinate coordinate) const {
  return coordinate.longitude >= min_lon and coordinate.longitude <= max_lon and
         coordinate.latitude >= min_lat and coordinate.latitude <= max_lat;
}

bool RectangleInt2D::is_valid() const {
  return min_lon != FixedLongitude{std::numeric_limits<std::int32_t>::max()} and
         max_lon != FixedLongitude{std::numeric_limits<std::int32_t>::min()} and
         min_lat != FixedLatitude{std::numeric_limits<std::int32_t>::max()} and
         max_lat != FixedLatitude{std::numeric_limits<std::int32_t>::min()};
}
} // namespace util
} // namespace osram
