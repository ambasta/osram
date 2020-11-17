#include "osram/util/typedefs.hxx"
#include <osram/extractor/restriction.hxx>

namespace osram {
namespace extractor {
OSMWayId InputTurnRestriction::from() const {
  return node_or_way.index() == 0
             ? std::get<InputNodeRestriction>(node_or_way).from
             : std::get<InputWayRestriction>(node_or_way).from;
}

OSMWayId InputTurnRestriction::to() const {
  return node_or_way.index() == 0
             ? std::get<InputNodeRestriction>(node_or_way).to
             : std::get<InputWayRestriction>(node_or_way).to;
}

RestrictionType InputTurnRestriction::type() const {
  assert(node_or_way.index() < RestrictionType::NUM_TYPES);
  return static_cast<RestrictionType>(node_or_way.index());
}

InputWayRestriction &InputTurnRestriction::as_way_restriction() {
  assert(node_or_way.index() == RestrictionType::WAY);
  return std::get<InputWayRestriction>(node_or_way);
}

const InputWayRestriction &InputTurnRestriction::as_way_restriction() const {
  assert(node_or_way.index() == RestrictionType::WAY);
  return std::get<InputWayRestriction>(node_or_way);
}

InputNodeRestriction &InputTurnRestriction::as_node_restriction() {
  assert(node_or_way.index() == RestrictionType::NODE);
  return std::get<InputNodeRestriction>(node_or_way);
}

const InputNodeRestriction &InputTurnRestriction::as_node_restriction() const {
  assert(node_or_way.index() == RestrictionType::NODE);
  return std::get<InputNodeRestriction>(node_or_way);
}

bool NodeRestriction::valid() const {
  return from != SPECIAL_NODEID and to != SPECIAL_NODEID and
         via != SPECIAL_NODEID;
}

bool NodeRestriction::operator==(const NodeRestriction &other) const {
  return std::tie(from, via, to) == std::tie(other.from, other.via, other.to);
}

bool WayRestriction::operator==(const WayRestriction &other) const {
  return std::tie(in_restriction, out_restriction) ==
         std::tie(other.in_restriction, other.out_restriction);
}

TurnRestriction::TurnRestriction(NodeRestriction restriction,
                                 bool is_only = false)
    : node_or_way(restriction), is_only(is_only) {}

TurnRestriction::TurnRestriction(WayRestriction restriction,
                                 bool is_only = false)
    : node_or_way(restriction), is_only(is_only) {}

TurnRestriction::TurnRestriction() {
  node_or_way = NodeRestriction{SPECIAL_EDGEID, SPECIAL_NODEID, SPECIAL_EDGEID};
}

WayRestriction &TurnRestriction::as_way_restriction() {
  assert(node_or_way.index() == RestrictionType::WAY);
  return std::get<WayRestriction>(node_or_way);
}

const WayRestriction &TurnRestriction::as_way_restriction() const {
  assert(node_or_way.index() == RestrictionType::WAY);
  return std::get<WayRestriction>(node_or_way);
}

NodeRestriction &TurnRestriction::as_node_restriction() {
  assert(node_or_way.index() == RestrictionType::NODE);
  return std::get<NodeRestriction>(node_or_way);
}

const NodeRestriction &TurnRestriction::as_node_restriction() const {
  assert(node_or_way.index() == RestrictionType::NODE);
  return std::get<NodeRestriction>(node_or_way);
}

RestrictionType TurnRestriction::type() const {
  assert(node_or_way.index() < RestrictionType::NUM_TYPES);
  return static_cast<RestrictionType>(node_or_way.index());
}

bool TurnRestriction::valid() const {
  if (node_or_way.index() == RestrictionType::WAY) {
    auto const &restriction = as_way_restriction();
    return restriction.in_restriction.valid() and
           restriction.out_restriction.valid();
  }

  auto const &restriction = as_node_restriction();
  return restriction.valid();
}

bool TurnRestriction::operator==(const TurnRestriction &other) const {
  if (is_only != other.is_only)
    return false;

  if (type() != other.type())
    return false;

  if (type() == RestrictionType::WAY)
    return as_way_restriction() == other.as_way_restriction();
  return as_node_restriction() == other.as_node_restriction();
}
} // namespace extractor
} // namespace osram
