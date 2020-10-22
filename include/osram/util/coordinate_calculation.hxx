#ifndef OSRAM_UTIL_COORDINATE_CALCULATION
#define OSRAM_UTIL_COORDINATE_CALCULATION
#include <osram/util/coordinate.hxx>

namespace osram {
namespace util {
namespace coordinate_calculation {
double haversine_distance(const Coordinate first, const Coordinate second);
}
} // namespace util
} // namespace osram
#endif
