#ifndef OSRAM_UTIL_RECTANGLE
#define OSRAM_UTIL_RECTANGLE

#include <cstdint>
#include <osram/util/coordinate.hxx>
#include <osram/util/coordinate_calculation.hxx>

namespace osram {
namespace util {
struct RectangleInt2D {
  FixedLongitude min_lon, max_lon;
  FixedLatitude min_lat, max_lat;

  RectangleInt2D();

  RectangleInt2D(FixedLongitude, FixedLongitude, FixedLatitude, FixedLatitude);

  RectangleInt2D(FloatLongitude, FloatLongitude, FloatLatitude, FloatLatitude);

  void merge_bounding_boxes(const RectangleInt2D &);

  Coordinate centroid() const;

  bool intersects(const RectangleInt2D &) const;

  std::uint64_t get_min_squared_dist(const Coordinate) const;

  bool contains(const Coordinate) const;

  bool is_valid() const;
};
} // namespace util
} // namespace osram
#endif
