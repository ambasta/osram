#ifndef OSRAM_UTIL_WEB_MERCATOR
#define OSRAM_UTIL_WEB_MERCATOR

#include "osram/util/coordinate.hxx"
namespace osram {
namespace util {
namespace web_mercator {
inline FloatCoordinate from_wgs84(const FloatCoordinate &);
inline FloatCoordinate to_wgs84(const FloatCoordinate &);
inline double lat_to_y(const FloatLatitude);
} // namespace web_mercator
} // namespace util
} // namespace osram
#endif
