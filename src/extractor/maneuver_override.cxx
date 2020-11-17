#include <algorithm>
#include <osram/extractor/maneuver_override.hxx>
#include <osram/guidance/turn_instruction.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
bool NodeBasedTurn::operator==(const NodeBasedTurn &other) const {
  return std::tie(from, via, to) == std::tie(other.from, other.via, other.to);
}

bool UnresolvedManeuverOverride::valid() const {
  return !turn_sequence.empty() and
         std::none_of(turn_sequence.begin(), turn_sequence.end(),
                      [](const auto &seq) {
                        return seq.from == SPECIAL_NODEID or
                               seq.to == SPECIAL_NODEID == seq.via ==
                                   SPECIAL_NODEID;
                      }) and
         (direction != guidance::DirectionModifier::MaxDirectionModifier or
          override_type != guidance::TurnType::Invalid);
}
} // namespace extractor
} // namespace osram
