#ifndef OSRAM_EXTRACTOR_INTERSECTION_INTERSECTION_EDGE
#define OSRAM_EXTRACTOR_INTERSECTION_INTERSECTION_EDGE

#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
namespace intersection {
struct IntersectionEdge {
  NodeId node;
  EdgeId edge;

  bool operator<(const IntersectionEdge &) const;
};

using IntersectionEdges = std::vector<IntersectionEdge>;

struct IntersectionEdgeGeomtry {
  EdgeId edge;
  double initial_bearing, perceived_bearing, segment_length;

  bool operator<(const IntersectionEdgeGeomtry &) const;
};

using IntersectionEdgeGeometries = std::vector<IntersectionEdgeGeomtry>;
} // namespace intersection
} // namespace extractor
} // namespace osram
#endif
