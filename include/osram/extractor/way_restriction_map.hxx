#ifndef OSRAM_EXTRACTOR_WAY_RESTRICTION_MAP
#define OSRAM_EXTRACTOR_WAY_RESTRICTION_MAP

#include <osram/extractor/restriction.hxx>
#include <osram/extractor/restriction_index.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
class WayRestrictionMap {
private:
  std::vector<DuplicatedNodeId> duplicated_node_groups;
  std::vector<ConditionalTurnRestriction> restriction_data;
  RestrictionIndex<ConditionalTurnRestriction> restriction_starts;

  DuplicatedNodeId as_duplicated_node_id(const RestrictionId) const;

public:
  struct ViaWay {
    NodeId from, to;
  };

  WayRestrictionMap(const std::vector<ConditionalTurnRestriction> &);

  bool is_via_way(const NodeId, const NodeId) const;

  std::size_t number_of_duplicate_nodes() const;

  std::vector<ViaWay> duplicated_node_representatives() const;

  std::vector<DuplicatedNodeId> duplicated_node_ids(const NodeId,
                                                   const NodeId) const;

  bool is_restricted(DuplicatedNodeId, const NodeId) const;

  const ConditionalTurnRestriction &get_restriction(DuplicatedNodeId,
                                                    const NodeId) const;

  NodeId remap_if_restricted(const NodeId, const NodeId, const NodeId,
                             const NodeId, const NodeId) const;
};
} // namespace extractor
} // namespace osram

#endif
