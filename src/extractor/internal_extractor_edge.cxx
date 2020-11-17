#include <cassert>
#include <osram/extractor/internal_extractor_edge.hxx>

namespace osram {
namespace extractor {
namespace detail {
ByEdgeOrMeterValue::ByEdgeOrMeterValue() : value(0.f) {}

ByEdgeOrMeterValue::ByEdgeOrMeterValue(ValueByEdge, double input) {
  assert(input >= 0.f);
  value = static_cast<value_type>(input);
}

ByEdgeOrMeterValue::ByEdgeOrMeterValue(ValueByMeter, double input) {
  assert(input > 0.f);
  value = -static_cast<value_type>(input);
}

double ByEdgeOrMeterValue::operator()(double distance) {
  return value >= 0 ? value : -distance / value;
}
} // namespace detail

InternalExtractorEdge::InternalExtractorEdge()
    : weight_data(), duration_data() {}

InternalExtractorEdge::InternalExtractorEdge(OSMNodeId source, OSMNodeId target,
                                             WeightData weight_data,
                                             DurationData duration_data,
                                             util::Coordinate source_coordinate)
    : result(source, target, 0, 0, 0, {}, -1, {}),
      weight_data(std::move(weight_data)),
      duration_data(std::move(duration_data)),
      source(std::move(source_coordinate)) {}

InternalExtractorEdge::InternalExtractorEdge(NodeBasedEdgeWithOSM edge,
                                             WeightData weight_data,
                                             DurationData duration_data,
                                             util::Coordinate source_coordinate)
    : result(std::move(edge)), weight_data(weight_data),
      duration_data(duration_data), source(source_coordinate) {}
} // namespace extractor
} // namespace osram
