#ifndef OSRAM_EXTRACTOR_MANEUVER_OVERRIDE
#define OSRAM_EXTRACTOR_MANEUVER_OVERRIDE

#include <string>
#include <utility>
#include <vector>

#include <osram/guidance/turn_instruction.hxx>
#include <osram/util/hash_combine.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
struct InputManeuverOverride {
  std::vector<OSMWayId> via_ways;
  OSMNodeId via_node;
  std::string maneuver, direction;
};

struct ManeuverOverride {
  std::vector<NodeId> node_sequence;
  NodeId instruction_node;
  guidance::TurnType override_type;
  guidance::DirectionModifier direction;
};

struct StorageManeuverOverride {
  std::uint32_t node_sequence_offset_begin, node_sequence_offset_end;
  NodeId start_node, instruction_node;
  guidance::TurnType override_type;
  guidance::DirectionModifier direction;
};

struct NodeBasedTurn {
  NodeId from, via, to;

  bool operator==(const NodeBasedTurn &) const;
};

struct UnresolvedManeuverOverride {
  std::vector<NodeBasedTurn> turn_sequence;
  NodeId instruction_node;
  guidance::TurnType override_type;
  guidance::DirectionModifier direction;

  bool valid() const;
};
} // namespace extractor
} // namespace osram

namespace std {
template <> struct hash<osram::extractor::NodeBasedTurn> {
  typedef osram::extractor::NodeBasedTurn arg_type;
  typedef std::size_t result_type;

  result_type operator()(arg_type const &arg) const noexcept {
    std::size_t seed = 0;
    osram::util::hash_combine(seed, arg.from);
    osram::util::hash_combine(seed, arg.via);
    osram::util::hash_combine(seed, arg.to);
    return seed;
  }
};
} // namespace std
#endif
