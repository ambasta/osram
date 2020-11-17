#include <osram/extractor/node_based_edge.hxx>
#include <tuple>

namespace osram {
namespace extractor {

inline NodeBasedEdgeClassification::NodeBasedEdgeClassification()
    : forward(false), backward(false), is_split(false), roundabout(false),
      circular(false), startpoint(false), restricted(false) {}

NodeBasedEdgeClassification::NodeBasedEdgeClassification(
    const bool forward, const bool backward, const bool is_split,
    const bool roundabout, const bool circular, const bool startpoint,
    const bool restricted, RoadClassification road_clasification,
    const std::uint8_t highway_turn_classification,
    const std::uint8_t access_turn_classification)
    : forward(forward), backward(backward), is_split(is_split),
      roundabout(roundabout), circular(circular), startpoint(startpoint),
      restricted(restricted), road_clasification(road_clasification),
      highway_turn_classification(highway_turn_classification),
      access_turn_classification(access_turn_classification) {}

bool NodeBasedEdgeClassification::operator==(
    const NodeBasedEdgeClassification &other) const {
  return std::tie(road_clasification, forward, backward, is_split, roundabout,
                  circular, startpoint, restricted) ==
         std::tie(other.road_clasification, other.forward, other.backward,
                  other.is_split, other.roundabout, other.circular,
                  other.startpoint, other.restricted);
}

bool NodeBasedEdgeAnnotation::can_combine_with(
    const NodeBasedEdgeAnnotation &other) const {
  return std::tie(name_id, classes, travel_mode, is_left_hand_driving) ==
         std::tie(other.name_id, other.classes, other.travel_mode,
                  other.is_left_hand_driving);
}

bool NodeBasedEdgeAnnotation::operator<(
    const NodeBasedEdgeAnnotation &other) const {
  return std::tie(name_id, lane_description_id, classes, travel_mode,
                  is_left_hand_driving) <
         std::tie(other.name_id, other.lane_description_id, other.classes,
                  other.travel_mode, other.is_left_hand_driving);
}

inline NodeBasedEdge::NodeBasedEdge()
    : source(SPECIAL_NODEID), target(SPECIAL_NODEID), weight(0), duration(0),
      distance(0), annotation_id(-1) {}

inline NodeBasedEdge::NodeBasedEdge(NodeId source, NodeId target,
                                    EdgeWeight weight, EdgeDuration duration,
                                    EdgeDistance distance,
                                    GeometryId geometry_id,
                                    AnnotationId annotation_id,
                                    NodeBasedEdgeClassification flags)
    : source(source), target(target), weight(weight), duration(duration),
      distance(distance), geometry_id(geometry_id),
      annotation_id(annotation_id), flags(flags) {}

inline bool NodeBasedEdge::operator<(const NodeBasedEdge &other) const {
  if (source == other.source) {
    if (target == other.target) {
      if (weight == other.weight) {
        return flags.forward and flags.backward and
               !(other.flags.forward and other.flags.backward);
      }
      return weight < other.weight;
    }
    return target < other.target;
  }
  return source < other.source;
}

inline NodeBasedEdgeWithOSM::NodeBasedEdgeWithOSM(
    OSMNodeId source, OSMNodeId target, EdgeWeight weight,
    EdgeDuration duration, EdgeDistance distance, GeometryId geometry_id,
    AnnotationId annotation_id, NodeBasedEdgeClassification flags)
    : NodeBasedEdge(SPECIAL_NODEID, SPECIAL_NODEID, weight, duration, distance,
                    geometry_id, annotation_id, flags),
      osm_source_id(std::move(source)), osm_target_id(std::move(target)) {}

inline NodeBasedEdgeWithOSM::NodeBasedEdgeWithOSM()
    : osm_source_id(MIN_OSM_NODEID), osm_target_id(MIN_OSM_NODEID) {}

static_assert(sizeof(NodeBasedEdge) == 48,
              "Size of extractor::NodeBasedEdge type is larger than expected. "
              "This will impact memory consumption");
} // namespace extractor
} // namespace osram
