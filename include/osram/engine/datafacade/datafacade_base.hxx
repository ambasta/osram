#ifndef OSRAM_ENGINE_DATAFACADE_DATAFACADE_BASE
#define OSRAM_ENGINE_DATAFACADE_DATAFACADE_BASE

#include <osram/engine/approach.hxx>
#include <osram/engine/phantom_node.hxx>

// #include <osram/contractor/query_edge.hxx>

#include <osram/extractor/class_data.hxx>
#include <osram/extractor/edge_based_node_segment.hxx>

#include <osram/extractor/segment_data_container.hxx>
#include <osram/extractor/turn_lane_types.hxx>

#include <osram/util/coordinate.hxx>

namespace osram {
namespace engine {
namespace datafacade {
class BaseDataFacade {
public:
  using RTreeLeaf = extractor::EdgeBasedNodeSegment;

  BaseDataFacade() {}

  virtual ~BaseDataFacade() {}

  virtual std::uint32_t get_checksum() const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const float, const int,
                                 const int, const Approach,
                                 const bool) const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const float,
                                 const Approach, const bool) const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const unsigned,
                                 const double, const int, const int,
                                 const Approach) const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const unsigned,
                                 const int, const int,
                                 const Approach) const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const unsigned,
                                 const Approach) const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const unsigned,
                                 const double, const Approach) const = 0;
};
} // namespace datafacade
} // namespace engine
} // namespace osram
#endif
