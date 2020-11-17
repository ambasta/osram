#ifndef OSRAM_EXTRACTOR_EDGE_BASED_NODE_SEGMENT
#define OSRAM_EXTRACTOR_EDGE_BASED_NODE_SEGMENT

#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
struct EdgeBasedNodeSegment {
  SegmentId forward_segment_id, reverse_segment_id;
  NodeId source, target;
  std::size_t fwd_segment_position;
  bool is_startpoint;

  EdgeBasedNodeSegment()
      : forward_segment_id{SPECIAL_SEGMENTID, false},
        reverse_segment_id{SPECIAL_SEGMENTID, false}, source(SPECIAL_NODEID),
        target(SPECIAL_NODEID),
        fwd_segment_position(std::numeric_limits<std::size_t>::max()),
        is_startpoint(false) {}

  explicit EdgeBasedNodeSegment(const SegmentId, const SegmentId, NodeId,
                                NodeId, unsigned short, bool);
};
} // namespace extractor
} // namespace osram
#endif
