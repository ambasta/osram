#ifndef OSRAM_ENGINE_PHANTOM_NODE
#define OSRAM_ENGINE_PHANTOM_NODE

#include <cmath>

#include <osram/util/bearing.hxx>
#include <osram/util/coordinate.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace engine {

struct PhantomNode {
private:
  unsigned short m_is_valid_forward_source : 1;
  unsigned short m_is_valid_forward_target : 1;
  unsigned short m_is_valid_reverse_source : 1;
  unsigned short m_is_valid_reverse_target : 1;
  unsigned short m_bearing : 12;

public:
  SegmentId forward_segment_id, reverse_segment_id;
  EdgeWeight forward_weight, reverse_weight, forward_weight_offset,
      reverse_weight_offset, forward_duration, reverse_duration,
      forward_duration_offset, reverse_duration_offset;
  EdgeDistance forward_distance, reverse_distance, forward_distance_offset,
      reverse_distance_offset;
  ComponentId component;

  util::Coordinate location; // this is the coordinate of x
  util::Coordinate input_location;

  unsigned short fwd_segment_position;

  PhantomNode()
      : forward_segment_id{SPECIAL_SEGMENTID, false},
        reverse_segment_id{SPECIAL_SEGMENTID, false},
        forward_weight(INVALID_EDGE_WEIGHT),
        reverse_weight(INVALID_EDGE_WEIGHT), forward_weight_offset(0),
        reverse_weight_offset(0), forward_distance(INVALID_EDGE_DISTANCE),
        reverse_distance(INVALID_EDGE_DISTANCE), forward_distance_offset(0),
        reverse_distance_offset(0), forward_duration(MAXIMAL_EDGE_DURATION),
        reverse_duration(MAXIMAL_EDGE_DURATION), forward_duration_offset(0),
        reverse_duration_offset(0), fwd_segment_position(0),
        m_is_valid_forward_source{false}, m_is_valid_forward_target{false},
        m_is_valid_reverse_source{false}, m_is_valid_reverse_target{false},
        m_bearing(0) {}

  EdgeWeight get_forward_weight_plus_offset() const {
    assert(forward_segment_id.enabled);
    return forward_weight_offset + forward_weight;
  }

  EdgeWeight get_reverse_weight_plus_offset() const {
    assert(reverse_segment_id.enabled);
    return reverse_weight_offset + reverse_weight;
  }

  EdgeWeight get_forward_duration() const {
    assert(forward_segment_id.enabled);
    return forward_duration_offset + forward_duration;
  }

  EdgeWeight get_reverse_duration() const {
    assert(reverse_segment_id.enabled);
    return reverse_duration_offset + reverse_duration;
  }

  EdgeDistance get_forward_distance() const {
    assert(forward_segment_id.enabled);
    return forward_distance_offset + forward_distance;
  }

  EdgeDistance get_reverse_distance() const {
    assert(reverse_segment_id.enabled);
    return reverse_distance_offset + reverse_distance;
  }

  bool is_bidirected() const {
    return forward_segment_id.enabled and reverse_segment_id.enabled;
  }

  bool is_valid(const unsigned number_of_nodes) const {
    return location.is_valid() and
           ((forward_segment_id.id < number_of_nodes) or
            (reverse_segment_id.id < number_of_nodes)) and
           ((forward_weight != INVALID_EDGE_WEIGHT) or
            (reverse_weight != INVALID_EDGE_WEIGHT)) and
           ((forward_duration != MAXIMAL_EDGE_DURATION) or
            (reverse_duration != MAXIMAL_EDGE_DURATION)) and
           ((forward_distance != INVALID_EDGE_DISTANCE) or
            (reverse_distance != INVALID_EDGE_DISTANCE)) and
           (component.id != INVALID_COMPONENTID);
  }

  bool is_valid(const unsigned number_of_nodes,
                const util::Coordinate queried_coordinate) const {
    return queried_coordinate == input_location and is_valid(number_of_nodes);
  }

  bool is_valid() const { return location.is_valid(); }

  bool is_valid_forward_source() const {
    return forward_segment_id.enabled and m_is_valid_forward_source;
  }

  bool is_valid_forward_target() const {
    return forward_segment_id.enabled and m_is_valid_forward_target;
  }

  bool is_valid_reverse_source() const {
    return reverse_segment_id.enabled and m_is_valid_reverse_source;
  }

  bool is_valid_reverse_target() const {
    return reverse_segment_id.enabled and m_is_valid_reverse_target;
  }

  short get_bearing(const bool traversed_in_reverse) const {
    if (traversed_in_reverse)
      return std::round(util::bearing::reverse(m_bearing));
    return std::round(m_bearing);
  }

  template <class Other>
  explicit PhantomNode(
      const Other &other, ComponentId component, EdgeWeight forward_weight,
      EdgeWeight reverse_weight, EdgeWeight forward_weight_offset,
      EdgeWeight reverse_weight_offset, EdgeDistance forward_distance,
      EdgeDistance reverse_distance, EdgeDistance forward_distance_offset,
      EdgeDistance reverse_distance_offset, EdgeWeight forward_duration,
      EdgeWeight reverse_duration, EdgeWeight forward_duration_offset,
      EdgeWeight reverse_duration_offset, bool is_valid_forward_source,
      bool is_valid_forward_target, bool is_valid_reverse_source,
      bool is_valid_reverse_target, const util::Coordinate location,
      const util::Coordinate input_location, const unsigned short bearing)
      : forward_segment_id{other.forward_segment_id},
        reverse_segment_id{other.reverse_segment_id},
        forward_weight{forward_weight}, reverse_weight{reverse_weight},
        forward_weight_offset{forward_weight_offset},
        reverse_weight_offset{reverse_weight_offset},
        forward_distance{forward_distance}, reverse_distance{reverse_distance},
        forward_distance_offset{forward_distance_offset},
        reverse_distance_offset{reverse_distance_offset},
        forward_duration{forward_duration}, reverse_duration{reverse_duration},
        forward_duration_offset{forward_duration_offset},
        reverse_duration_offset{reverse_duration_offset},
        component{component.id, component.is_tiny}, location{location},
        input_location{input_location},
        fwd_segment_position{other.fwd_segment_position},
        m_is_valid_forward_source{is_valid_forward_source},
        m_is_valid_forward_target{is_valid_forward_target},
        m_is_valid_reverse_source{is_valid_reverse_source},
        m_is_valid_reverse_target{is_valid_reverse_target}, m_bearing{bearing} {
  }
};

static_assert(sizeof(PhantomNode) == 84,
              "PhantomNode has more padding then expected");

using PhantomNodePair = std::pair<PhantomNode, PhantomNode>;

struct PhantomNodeWithDistance {
  PhantomNode phantom_node;
  double distance;
};

struct PhantomNodes {
  PhantomNode source, target;
};
} // namespace engine
} // namespace osram
#endif
