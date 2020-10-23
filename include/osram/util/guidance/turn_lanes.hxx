#ifndef OSRAM_UTIL_GUIDANCE_TURN_LANES
#define OSRAM_UTIL_GUIDANCE_TURN_LANES

#include <osram/util/concurrent_id_map.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace util {
namespace guidance {
class LaneTuple {
public:
  LaneId lanes_in_turn, first_lane_from_the_right;

  LaneTuple();
  LaneTuple(const LaneId, const LaneId);

  bool operator==(const LaneTuple) const;
};

class LaneTupleIdPair {
public:
  LaneTuple first;
  LaneDescriptionId second;

  bool operator==(const LaneTupleIdPair &) const;
};

using LaneDataIdMap = ConcurrentIdMap<LaneTupleIdPair, LaneDataId>;
} // namespace guidance
} // namespace util
} // namespace osram
#endif
