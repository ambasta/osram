#ifndef OSRAM_ENGINE_INTERNAL_ROUTE_RESULT
#define OSRAM_ENGINE_INTERNAL_ROUTE_RESULT

#include <cassert>
#include <osram/extractor/class_data.hxx>
#include <osram/extractor/travel_mode.hxx>

#include <osram/guidance/turn_bearing.hxx>
#include <osram/guidance/turn_instruction.hxx>

#include <osram/engine/phantom_node.hxx>

#include <osram/util/coordinate.hxx>
#include <osram/util/guidance/entry_class.hxx>
#include <osram/util/guidance/turn_lanes.hxx>
// #include <osram/util/integer_range.hxx>
#include <osram/util/typedefs.hxx>

#include <vector>

#include <range/v3/view/iota.hpp>

namespace osram {
namespace engine {
struct PathData {
  NodeId from_edge_based_node, turn_via_node;
  unsigned name_id;
  bool is_segregated;

  EdgeWeight weight_until_turn, weight_of_turn, duration_until_turn,
      duration_of_turn;

  guidance::TurnInstruction turn_instruction;

  util::guidance::LaneTupleIdPair lane_data;

  extractor::TravelMode travel_mode : 4;
  extractor::ClassData classes;

  util::guidance::EntryClass entry_class;

  DataSourceId datasource_id;

  osram::guidance::TurnBearing pre_turn_bearing, post_turn_bearing;

  bool is_left_hand_driving;
};

struct InternalRouteResult {
  std::vector<std::vector<PathData>> unpacked_path_segments;
  std::vector<PhantomNodes> segment_end_coordinates;
  std::vector<bool> source_traversed_in_reverse, target_traversed_in_reverse;
  EdgeWeight shortes_path_weight = INVALID_EDGE_WEIGHT;

  bool is_valid() const { return INVALID_EDGE_WEIGHT != shortes_path_weight; }

  bool is_via_leg(const std::size_t leg) const {
    return leg != unpacked_path_segments.size() - 1;
  }

  EdgeWeight duration() const {
    EdgeWeight ret{0};

    for (const auto &leg : unpacked_path_segments) {
      for (const auto &segment : leg) {
        ret += segment.duration_until_turn;
      }
    }
    return ret;
  }
};

struct InternalManyRoutesResult {
  std::vector<InternalRouteResult> routes;

  InternalManyRoutesResult() = default;
  InternalManyRoutesResult(InternalRouteResult route)
      : routes{std::move(route)} {}
  InternalManyRoutesResult(std::vector<InternalRouteResult> routes)
      : routes{std::move(routes)} {}
};

inline InternalRouteResult
collapse_internal_route_result(const InternalRouteResult &leg_result,
                               const std::vector<bool> &is_waypoint) {
  assert(leg_result.is_valid());
  assert(is_waypoint[0]);
  assert(is_waypoint.back());

  if (leg_result.unpacked_path_segments.size() == 1)
    return leg_result;

  assert(leg_result.segment_end_coordinates.size() > 1);

  InternalRouteResult collapsed;
  collapsed.shortes_path_weight = leg_result.shortes_path_weight;

  for (size_t index = 0; index < leg_result.unpacked_path_segments.size();
       ++index) {
    if (is_waypoint[index]) {
      collapsed.unpacked_path_segments.push_back(
          leg_result.unpacked_path_segments[index]);
      collapsed.segment_end_coordinates.push_back(
          leg_result.segment_end_coordinates[index]);
      collapsed.source_traversed_in_reverse.push_back(
          leg_result.source_traversed_in_reverse[index]);
      collapsed.target_traversed_in_reverse.push_back(
          leg_result.target_traversed_in_reverse[index]);
    } else {
      assert(!collapsed.unpacked_path_segments.empty());
      auto &last_segment = collapsed.unpacked_path_segments.back();
      assert(!collapsed.segment_end_coordinates.empty());
      collapsed.segment_end_coordinates.back().target =
          leg_result.segment_end_coordinates[index].target;
      collapsed.target_traversed_in_reverse.back() =
          leg_result.target_traversed_in_reverse[index];

      if (!leg_result.unpacked_path_segments[index].empty()) {
        auto old_size = last_segment.size();
        last_segment.insert(last_segment.end(),
                            leg_result.unpacked_path_segments[index].begin(),
                            leg_result.unpacked_path_segments[index].end());
        last_segment[old_size].weight_until_turn =
            leg_result.source_traversed_in_reverse[index]
                ? leg_result.segment_end_coordinates[index]
                      .source.reverse_weight
                : leg_result.segment_end_coordinates[index]
                      .source.forward_weight;

        last_segment[old_size].duration_until_turn =
            leg_result.source_traversed_in_reverse[index]
                ? leg_result.segment_end_coordinates[index]
                      .source.reverse_duration
                : leg_result.segment_end_coordinates[index]
                      .source.forward_duration;
      }
    }
  }

  assert(collapsed.segment_end_coordinates.size() ==
         collapsed.unpacked_path_segments.size());
  return collapsed;
}
} // namespace engine
} // namespace osram
#endif
