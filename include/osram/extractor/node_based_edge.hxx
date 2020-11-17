#ifndef OSRAM_EXTRACTOR_NODE_BASED_EDGE
#define OSRAM_EXTRACTOR_NODE_BASED_EDGE
#include <osram/extractor/class_data.hxx>
#include <osram/extractor/road_classification.hxx>
#include <osram/extractor/travel_mode.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
struct NodeBasedEdgeClassification {
  bool forward, backward, is_split, roundabout, circular, startpoint,
      restricted;
  RoadClassification road_clasification;
  std::uint8_t highway_turn_classification, access_turn_classification;

  NodeBasedEdgeClassification();

  NodeBasedEdgeClassification(const bool, const bool, const bool, const bool,
                              const bool, const bool, const bool,
                              RoadClassification, const std::uint8_t,
                              const std::uint8_t);

  bool operator==(const NodeBasedEdgeClassification &) const;
};

struct NodeBasedEdgeAnnotation {
  NameId name_id;
  LaneDescriptionId lane_description_id;
  ClassData classes;
  TravelMode travel_mode;
  bool is_left_hand_driving;

  bool can_combine_with(const NodeBasedEdgeAnnotation &) const;

  bool operator<(const NodeBasedEdgeAnnotation &) const;
};

struct NodeBasedEdge {
  NodeId source, target;
  EdgeWeight weight;
  EdgeDuration duration;
  EdgeDistance distance;
  GeometryId geometry_id;
  AnnotationId annotation_id;
  NodeBasedEdgeClassification flags;

  NodeBasedEdge();

  NodeBasedEdge(NodeId, NodeId, EdgeWeight, EdgeDuration, EdgeDistance,
                GeometryId, AnnotationId, NodeBasedEdgeClassification);

  bool operator<(const NodeBasedEdge &) const;
};

struct NodeBasedEdgeWithOSM : NodeBasedEdge {
  OSMNodeId osm_source_id, osm_target_id;

  NodeBasedEdgeWithOSM();

  NodeBasedEdgeWithOSM(OSMNodeId, OSMNodeId, EdgeWeight, EdgeDuration,
                       EdgeDistance, GeometryId, AnnotationId,
                       NodeBasedEdgeClassification);
};
} // namespace extractor
} // namespace osram
#endif
