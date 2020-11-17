#ifndef OSRAM_EXTRACTOR_RESTRICTION
#define OSRAM_EXTRACTOR_RESTRICTION

#include <variant>

#include <osram/util/coordinate.hxx>
#include <osram/util/typedefs.hxx>
#include <osram/util/working_hours.hxx>

namespace osram {
namespace extractor {
struct InputNodeRestriction {
  OSMWayId from, to;
  OSMNodeId via;
};

struct InputWayRestriction {
  OSMWayId from, via, to;
};

enum RestrictionType { NODE = 0, WAY = 1, NUM_TYPES = 2 };

struct InputTurnRestriction {
  std::variant<InputNodeRestriction, InputWayRestriction> node_or_way;
  bool is_only;

  OSMWayId from() const;

  OSMWayId to() const;

  RestrictionType type() const;

  InputWayRestriction &as_way_restriction();
  const InputWayRestriction &as_way_restriction() const;

  InputNodeRestriction &as_node_restriction();
  const InputNodeRestriction &as_node_restriction() const;
};

struct InputConditionalTurnRestriction : InputTurnRestriction {
  std::vector<util::WorkingHours> condition;
};

struct NodeRestriction {
  NodeId from, via, to;

  bool valid() const;

  bool operator==(const NodeRestriction &) const;
};

struct WayRestriction {
  NodeRestriction in_restriction, out_restriction;

  bool operator==(const WayRestriction &) const;
};

struct TurnRestriction {
  std::variant<NodeRestriction, WayRestriction> node_or_way;
  bool is_only;

  explicit TurnRestriction(NodeRestriction, bool);

  explicit TurnRestriction(WayRestriction, bool);

  explicit TurnRestriction();

  WayRestriction &as_way_restriction();

  NodeRestriction &as_node_restriction();

  const WayRestriction &as_way_restriction() const;

  const NodeRestriction &as_node_restriction() const;

  RestrictionType type() const;

  bool valid() const;

  bool operator==(const TurnRestriction &) const;
};

struct ConditionalTurnRestriction : TurnRestriction {
  std::vector<util::WorkingHours> condition;
};
} // namespace extractor
} // namespace osram
#endif
