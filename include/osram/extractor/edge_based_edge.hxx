#ifndef OSRAM_EXTRACTOR_EDGE_BASED_EDGE
#define OSRAM_EXTRACTOR_EDGE_BASED_EDGE

#include <osram/extractor/travel_mode.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
struct EdgeBasedEdge {
public:
  struct EdgeData {
    NodeId turn_id;
    EdgeWeight weight, duration : 30;
    EdgeDistance distance;
    std::uint32_t forward : 1;
    std::uint32_t backward : 1;

    EdgeData()
        : turn_id(0), weight(0), distance(0), duration(0), forward(false),
          backward(false) {}

    EdgeData(const NodeId turn_id, const EdgeWeight weight,
             const EdgeDistance distance, const EdgeWeight duration,
             const bool forward, const bool backward)
        : turn_id(turn_id), weight(weight), distance(distance),
          duration(duration), forward(forward), backward(backward) {}

    auto is_unidirectional() const { return !forward or !backward; }
  };

  NodeId source, target;
  EdgeData data;

  EdgeBasedEdge();

  template <class E> explicit EdgeBasedEdge(const E &other);

  EdgeBasedEdge(const NodeId source, const NodeId target, const NodeId edge_id,
                const EdgeWeight weight, const EdgeWeight duration,
                const EdgeDistance distance, const bool forward,
                const bool backward);

  EdgeBasedEdge(const NodeId source, const NodeId target,
                const EdgeBasedEdge::EdgeData &data);

  bool operator<(const EdgeBasedEdge &other) const;
};

static_assert(sizeof(extractor::EdgeBasedEdge) == 28,
              "Size of extractor::EdgeBasedEdge type is bigger than expected. "
              "This will influence memory consumption.");

inline EdgeBasedEdge::EdgeBasedEdge() : source(0), target(0) {}

inline EdgeBasedEdge::EdgeBasedEdge(const NodeId source, const NodeId target,
                                    const NodeId turn_id,
                                    const EdgeWeight weight,
                                    const EdgeWeight duration,
                                    const EdgeDistance distance,
                                    const bool forward, const bool backward)
    : source(source), target(target), data{turn_id,  weight,  distance,
                                           duration, forward, backward} {}

inline EdgeBasedEdge::EdgeBasedEdge(const NodeId source, const NodeId target,
                                    const EdgeBasedEdge::EdgeData &data)
    : source(source), target(target), data{data} {}

inline bool EdgeBasedEdge::operator<(const EdgeBasedEdge &other) const {
  const auto unidirectional = data.is_unidirectional();
  const auto other_is_unidirectional = other.data.is_unidirectional();
  return std::tie(source, target, data.weight, unidirectional) <
         std::tie(other.source, other.target, other.data.weight,
                  other_is_unidirectional);
}
} // namespace extractor
} // namespace osram
#endif
