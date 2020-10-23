#include <osram/util/guidance/turn_lanes.hxx>

namespace osram {
namespace util {
namespace guidance {
LaneTuple::LaneTuple()
    : lanes_in_turn(0), first_lane_from_the_right(INVALID_LANEID) {}

LaneTuple::LaneTuple(const LaneId lanes_in_turn,
                     const LaneId first_lane_from_the_right)
    : lanes_in_turn(lanes_in_turn),
      first_lane_from_the_right(first_lane_from_the_right) {}

bool LaneTuple::operator==(const LaneTuple other) const {
  return std::tie(lanes_in_turn, first_lane_from_the_right) ==
         std::tie(other.lanes_in_turn, other.first_lane_from_the_right);
}

bool LaneTupleIdPair::operator==(const LaneTupleIdPair &other) const {
  return other.first == first and other.second == second;
}
} // namespace guidance
} // namespace util
} // namespace osram
