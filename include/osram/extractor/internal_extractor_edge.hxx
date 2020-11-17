#ifndef OSRAM_EXTRACTOR_INTERNAL_EXTRACTOR_EDGE
#define OSRAM_EXTRACTOR_INTERNAL_EXTRACTOR_EDGE

#include <osram/util/coordinate.hxx>
#include <osram/util/typedefs.hxx>
#include <osram/extractor/node_based_edge.hxx>
#include <osram/extractor/travel_mode.hxx>

namespace osram {
namespace extractor {
namespace detail {
struct ByEdgeOrMeterValue {
  struct ValueByEdge {
  } static const by_edge;
  struct ValueByMeter {
  } static const by_meter;

  ByEdgeOrMeterValue();

  ByEdgeOrMeterValue(ValueByEdge, double);

  ByEdgeOrMeterValue(ValueByMeter, double);

  double operator()(double);

private:
  using value_type = float;
  value_type value;
};
} // namespace detail

struct InternalExtractorEdge {
    using WeightData = detail::ByEdgeOrMeterValue;
    using DurationData = detail::ByEdgeOrMeterValue;

    NodeBasedEdgeWithOSM result;
    WeightData weight_data;
    DurationData duration_data;
    util::Coordinate source;

    explicit InternalExtractorEdge();

    explicit InternalExtractorEdge(OSMNodeId, OSMNodeId, WeightData, DurationData, util::Coordinate);

    explicit InternalExtractorEdge(NodeBasedEdgeWithOSM, WeightData, DurationData, util::Coordinate);
};
} // namespace extractor
} // namespace osram
#endif
