#include <algorithm>
#include <iterator>
#include <numeric>
#include <range/v3/view/zip.hpp>
#include <tuple>
#include <utility>

#include <osram/extractor/restriction.hxx>
#include <osram/extractor/way_restriction_map.hxx>
#include <osram/util/typedefs.hxx>

#include <ranges>

namespace osram {
namespace extractor {
namespace {
struct find_via_way {
  bool operator()(const std::tuple<NodeId, NodeId> value,
                  const TurnRestriction &restriction) const {
    const auto &way = restriction.as_way_restriction();
    return value < std::tie(way.in_restriction.via, way.out_restriction.via);
  }

  bool operator()(const TurnRestriction &restriction,
                  const std::tuple<NodeId, NodeId> value) const {
    const auto &way = restriction.as_way_restriction();
    return std::tie(way.in_restriction.via, way.out_restriction.via) < value;
  }
};

template <typename T> auto as_duplicated_node(const T &restriction) {
  auto &way = restriction.as_way_restriction();
  return std::tie(way.in_restriction.via, way.out_restriction.via,
                  way.in_restriction.from);
}

template <typename T> struct compare_by_duplicated_node {
  bool operator()(const ConditionalTurnRestriction &lhs,
                  const ConditionalTurnRestriction &rhs) {
    if (as_duplicated_node(lhs) < as_duplicated_node(rhs))
      return true;
    if (as_duplicated_node(rhs) < as_duplicated_node(lhs))
      return false;

    const auto lhs_to = lhs.as_way_restriction().out_restriction.to;
    const auto rhs_to = rhs.as_way_restriction().out_restriction.to;

    const bool has_conditions_lhs = !lhs.condition.empty();
    const bool has_conditions_rhs = !rhs.condition.empty();

    return std::tie(lhs_to, lhs.is_only, has_conditions_lhs) <
           std::tie(rhs_to, rhs.is_only, has_conditions_rhs);
  }
};

template <typename T>
std::vector<T> extract_restrictions(const std::vector<T> &turn_restrictions) {
  std::vector<T> result;

  for (const auto &turn_restriction : turn_restrictions) {
    if (turn_restriction.type() == RestrictionType::WAY) {
      const auto &way = turn_restriction.as_way_restriction();

      if (way.in_restriction.via == way.out_restriction.from and
          way.in_restriction.to == way.out_restriction.via)
        result.push_back(turn_restriction);
    }
  }

  std::sort(result.begin(), result.end(), compare_by_duplicated_node<T>());
  auto new_end = std::unique(result.begin(), result.end());
  result.erase(new_end, result.end());
  return result;
}

template <typename T> struct by_in_from_and_via {
  std::pair<NodeId, NodeId> operator()(const T &restriction) {
    const auto &way = restriction.as_way_restriction();
    return std::make_pair(way.in_restriction.from, way.in_restriction.via);
  }
};
} // namespace

WayRestrictionMap::WayRestrictionMap(
    const std::vector<ConditionalTurnRestriction> &turn_restrictions)
    : restriction_data(extract_restrictions(turn_restrictions)),
      restriction_starts(restriction_data,
                         by_in_from_and_via<ConditionalTurnRestriction>()) {
  std::size_t offset = 1;

  if (!restriction_data.empty())
    duplicated_node_groups.push_back(0);

  auto const add_offset_on_new_groups = [&](auto const &lhs, auto const &rhs) {
    assert(rhs == restriction_data[offset]);
    assert(lhs.type() == RestrictionType::WAY);
    assert(rhs.type() == RestrictionType::WAY);

    if (as_duplicated_node(lhs) != as_duplicated_node(rhs))
      duplicated_node_groups.push_back(offset);
    ++offset;
  };

  auto pair_view = ranges::views::zip(
      restriction_data | ranges::views::take(restriction_data.size() - 1),
      restriction_data | ranges::views::drop(1));

  for (auto &&pair : pair_view) {
    add_offset_on_new_groups(pair.first, pair.second);
  }
  duplicated_node_groups.push_back(restriction_data.size());
}

std::size_t WayRestrictionMap::number_of_duplicate_nodes() const {
  return duplicated_node_groups.size() - 1;
}

bool WayRestrictionMap::is_via_way(const NodeId from, const NodeId to) const {
  if (restriction_data.empty())
    return false;

  const auto iter =
      std::lower_bound(restriction_data.begin(), restriction_data.end(),
                       std::make_tuple(from, to), find_via_way());

  if (iter == restriction_data.end())
    return false;

  const auto &way = iter->as_way_restriction();
  return way.out_restriction.from == from and way.out_restriction.via == to;
}

DuplicatedNodeId WayRestrictionMap::as_duplicated_node_id(
    const RestrictionId restriction_id) const {
  const auto upper_bound_restriction =
      std::upper_bound(duplicated_node_groups.begin(),
                       duplicated_node_groups.end(), restriction_id);
  const auto distance_to_upper_bound =
      std::distance(duplicated_node_groups.begin(), upper_bound_restriction);
  return distance_to_upper_bound - 1;
}

std::vector<DuplicatedNodeId>
WayRestrictionMap::duplicated_node_ids(const NodeId from,
                                       const NodeId to) const {
  const auto duplicated_node_range_iter =
      std::equal_range(restriction_data.begin(), restriction_data.end(),
                       std::make_tuple(from, to), find_via_way());
  const auto as_restriction_id = [this](const auto iter) {
    return std::distance(restriction_data.begin(), iter);
  };

  auto first = as_duplicated_node_id(
      as_restriction_id(duplicated_node_range_iter.first));
  auto last = as_duplicated_node_id(
      as_restriction_id(duplicated_node_range_iter.second));

  std::vector<DuplicatedNodeId> result(last - first);
  std::iota(result.begin(), result.end(), first);
  return result;
}

bool WayRestrictionMap::is_restricted(DuplicatedNodeId duplicated_node,
                                      const NodeId to) const {
  for (RestrictionId restriction_index =
           duplicated_node_groups[duplicated_node];
       restriction_index != duplicated_node_groups[duplicated_node + 1];
       ++restriction_index) {
    assert(restriction_index < restriction_data.size());
    const auto &restriction = restriction_data[restriction_index];
    const auto &way = restriction.as_way_restriction();

    if (restriction.is_only)
      return way.out_restriction.to != to;

    if (to == way.out_restriction.to)
      return true;
  }
  return false;
}

ConditionalTurnRestriction const &
WayRestrictionMap::get_restriction(DuplicatedNodeId dupliated_node,
                                   const NodeId to) const {
  for (RestrictionId restriction_index = duplicated_node_groups[dupliated_node];
       restriction_index != duplicated_node_groups[dupliated_node + 1];
       ++restriction_index) {
    assert(restriction_index < restriction_data.size());
    const auto &restriction = restriction_data[restriction_index];
    const auto &way = restriction.as_way_restriction();

    if (restriction.is_only and way.out_restriction.to != to)
      return restriction;

    if (!restriction.is_only and to == way.out_restriction.to)
      return restriction;
  }

  throw std::runtime_error(
      "Asking for the restriction of an unrestricted turn. Check with "
      "is_restricted before calling get_restriction");
}

std::vector<WayRestrictionMap::ViaWay>
WayRestrictionMap::duplicated_node_representatives() const {
  std::vector<ViaWay> result;
  result.reserve(number_of_duplicate_nodes());

  std::transform(
      duplicated_node_groups.begin(), duplicated_node_groups.end() - 1,
      std::back_inserter(result), [&](auto const representative_id) -> ViaWay {
        auto &way = restriction_data[representative_id].as_way_restriction();
        return {way.in_restriction.via, way.out_restriction.via};
      });
  return result;
}

NodeId WayRestrictionMap::remap_if_restricted(
    const NodeId edge_based_node, const NodeId node_based_from,
    const NodeId node_based_via, const NodeId node_based_to,
    const NodeId number_of_edge_based_nodes) const {
  auto range = restriction_starts.restrictions(node_based_from, node_based_via);
  const auto restriction_targets_to = [node_based_to](const auto &pair) {
    return pair.second->as_way_restriction().in_restriction.to == node_based_to;
  };
  const auto iter =
      std::find_if(range.first, range.second, restriction_targets_to);

  if (iter != range.second)
    return number_of_edge_based_nodes - number_of_duplicate_nodes() +
           as_duplicated_node_id(iter->second - &restriction_data[0]);
  return edge_based_node;
}
} // namespace extractor
} // namespace osram
