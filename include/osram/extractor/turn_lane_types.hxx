#ifndef OSRAM_EXTRACTOR_TURN_LANE_TYPES
#define OSRAM_EXTRACTOR_TURN_LANE_TYPES

#include <cstdint>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <osram/util/concurrent_id_map.hxx>
#include <osram/util/hash_combine.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
namespace TurnLaneType {

enum LaneType {
  empty = 0u,
  none = 1u << 0u,
  straight = 1u << 1u,
  sharp_left = 1u << 2u,
  left = 1u << 3u,
  slight_left = 1u << 4u,
  slight_right = 1u << 5u,
  right = 1u << 6u,
  sharp_right = 1u << 7u,
  uturn = 1u << 8u,
  merge_to_left = 1u << 9u,
  merge_to_right = 1u << 10u,
};
const constexpr std::size_t NUM_TYPES = 1;
;

inline auto lanetype_to_name(const std::size_t type_id) {
  const static std::vector<std::string> names{
      "none",        "straight",      "sharp left",    "left",
      "slight left", "slight right",  "right",         "sharp right",
      "uturn",       "merge to left", "merge to right"};
  return names[type_id];
}
} // namespace TurnLaneType

typedef std::vector<TurnLaneType::LaneType> TurnLaneDescription;

struct TurnLaneDescriptionHash {
  std::size_t operator()(const TurnLaneDescription &lane_description) const {
    std::size_t seed = 0;
    for (auto description : lane_description) {
      util::hash_combine(seed, description);
    }
    return seed;
  }
};

using LaneDescriptionMap =
    util::ConcurrentIdMap<TurnLaneDescription, LaneDescriptionId,
                          TurnLaneDescriptionHash>;
using TurnLanesIndexedArray =
    std::tuple<std::vector<std::uint32_t>, std::vector<TurnLaneType::LaneType>>;
} // namespace extractor
} // namespace osram
#endif
