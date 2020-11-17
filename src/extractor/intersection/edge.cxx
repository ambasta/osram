#include <osram/extractor/intersection/edge.hxx>

namespace osram {
namespace extractor {
namespace intersection {
bool IntersectionEdge::operator<(const IntersectionEdge &other) const {
  return std::tie(node, edge) < std::tie(other.node, other.edge);
}

bool IntersectionEdgeGeomtry::operator<(
    const IntersectionEdgeGeomtry &other) const {
  return edge < other.edge;
}
} // namespace intersection
} // namespace extractor
} // namespace osram
