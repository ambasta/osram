#ifndef OSRAM_EXTRACTOR_RESTRICTION_INDEX
#define OSRAM_EXTRACTOR_RESTRICTION_INDEX
#include <osram/extractor/restriction.hxx>
#include <osram/util/typedefs.hxx>
#include <unordered_map>
#include <vector>

namespace osram {
namespace extractor {
template <typename T> class RestrictionIndex {
private:
  std::unordered_map<std::pair<NodeId, NodeId>, T *> restriction_hash;

public:
  using value_type = T;

  template <typename E> RestrictionIndex(std::vector<T> &, E);

  bool is_indexed(NodeId, NodeId) const;

  auto restrictions(NodeId first, NodeId second) const {
    return restriction_hash.equal_range(std::make_pair(first, second));
  }

  std::size_t size() const { return restriction_hash.size(); }
};

template <typename T>
template <typename E>
RestrictionIndex<T>::RestrictionIndex(std::vector<T> &restrictions,
                                      E extractor) {
  for (auto &restriction : restrictions) {
    restriction_hash.insert(
        std::make_pair(extractor(restriction), &restriction));
  }
}

template <typename T>
bool RestrictionIndex<T>::is_indexed(const NodeId first,
                                     const NodeId second) const {
  return restriction_hash.count(std::make_pair(first, second));
}

struct IndexNodeByFromAndVia {
  std::pair<NodeId, NodeId> operator()(const TurnRestriction &restriction) {
    const auto &node = restriction.as_node_restriction();
    return std::make_pair(node.from, node.via);
  }
};

template <typename T>
std::pair<bool, typename T::value_type *>
is_restricted(const NodeId from, const NodeId via, const NodeId to,
              const T &restriction_map) {
  const auto range = restriction_map.restrictions(from, via);

  const auto to_is_restricted = [to](const auto &pair) {
    const auto &restriction = *pair.second;

    if (restriction.type() == RestrictionType::NODE) {
      auto const &as_node = restriction.as_node_restriction();
      auto const restricted =
          restriction.is_only ? (to != as_node.to) : (to == as_node.to);

      return restricted;
    }
    return false;
  };

  auto iter = std::find_if(range.first, range.second, to_is_restricted);

  if (iter != range.second)
    return {true, iter->second};
  return {false, NULL};
}

using RestrictionMap = RestrictionIndex<TurnRestriction>;
using ConditionalRestrictionMap = RestrictionIndex<ConditionalTurnRestriction>;
} // namespace extractor
} // namespace osram
#endif
