#ifndef OSRAM_EXTRACTOR_CONDITIONAL_TURN_PENALTY
#define OSRAM_EXTRACTOR_CONDITIONAL_TURN_PENALTY
#include <cstdint>

#include <osram/util/coordinate.hxx>
#include <osram/util/working_hours.hxx>

namespace osram {
namespace extractor {
struct ConditionalTurnPenalty {
  std::uint64_t turn_offset;
  util::Coordinate location;
  std::vector<util::WorkingHours> conditions;
};
} // namespace extractor
} // namespace osram
#endif
