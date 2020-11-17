#ifndef OSRAM_EXTRACTOR_QUERY_NODE
#define OSRAM_EXTRACTOR_QUERY_NODE

#include <cstdint>
#include <osram/util/coordinate.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
struct QueryNode {
  using key_type = OSMNodeId;
  using value_type = std::int32_t;

  util::FixedLongitude lon;
  util::FixedLatitude lat;
  key_type node_id;

  explicit QueryNode(const util::FixedLongitude lon,
                     const util::FixedLatitude lat, const key_type node_id)
      : lon(lon), lat(lat), node_id(node_id) {}

  QueryNode()
      : lon{std::numeric_limits<value_type>::max()},
        lat{std::numeric_limits<value_type>::max()}, node_id{
                                                         SPECIAL_OSM_NODEID} {}
  static QueryNode min() {
    return QueryNode(util::FixedLongitude{static_cast<value_type>(
                         -180 * COORDINATE_PRECISION)},
                     util::FixedLatitude{
                         static_cast<value_type>(-90 * COORDINATE_PRECISION)},
                     MIN_OSM_NODEID);
  }

  static QueryNode max() {
    return QueryNode(
        util::FixedLongitude{
            static_cast<value_type>(180 * COORDINATE_PRECISION)},
        util::FixedLatitude{static_cast<value_type>(90 * COORDINATE_PRECISION)},
        MAX_OSM_NODEID);
  }
};
} // namespace extractor
} // namespace osram
#endif
