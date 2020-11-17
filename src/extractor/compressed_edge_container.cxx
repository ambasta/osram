#include "osram/util/typedefs.hxx"
#include <algorithm>
#include <fmt/core.h>
#include <osram/extractor/compressed_edge_container.hxx>

namespace osram {
namespace extractor {
CompressedEdgeContainer::CompressedEdgeContainer() {
  m_free_list.reserve(100);
  increase_free_list();
}

void CompressedEdgeContainer::increase_free_list() {
  m_compressed_oneway_geometries.resize(m_compressed_oneway_geometries.size() +
                                        100);

  for (std::size_t index = 100; index > 0; --index) {
    m_free_list.emplace_back(free_lis_max);
    ++free_lis_max;
  }
}

bool CompressedEdgeContainer::has_entry_for_id(const EdgeId edge_id) const {
  auto iter = m_edge_id_to_list_index_map.find(edge_id);
  return iter != m_edge_id_to_list_index_map.end();
}

bool CompressedEdgeContainer::has_zipped_entry_for_forward_id(
    const EdgeId edge_id) const {
  auto iter = m_forward_edge_id_to_zipped_index_map.find(edge_id);
  return iter != m_forward_edge_id_to_zipped_index_map.end();
}

bool CompressedEdgeContainer::has_zipped_entry_for_reverse_id(
    const EdgeId edge_id) const {
  auto iter = m_reverse_edge_id_to_zipped_index_map.find(edge_id);
  return iter != m_reverse_edge_id_to_zipped_index_map.end();
}

unsigned
CompressedEdgeContainer::get_position_for_id(const EdgeId edge_id) const {
  auto iter = m_edge_id_to_list_index_map.find(edge_id);
  assert(iter != m_edge_id_to_list_index_map.end());
  assert(iter->second < m_compressed_oneway_geometries.size());
  return iter->second;
}

/* unsigned CompressedEdgeContainer::get_zipped_position_for_forward_id(
    const EdgeId edge_id) const {
  auto iter = m_forward_edge_id_to_zipped_index_map.find(edge_id);
  assert(iter != m_forward_edge_id_to_zipped_index_map.end());
  assert(iter->second < segment_data->nodes_size());
  return iter->second;
}

unsigned CompressedEdgeContainer::get_zipped_position_for_reverse_id(
    const EdgeId edge_id) const {
  auto iter = m_reverse_edge_id_to_zipped_index_map.find(edge_id);
  assert(iter != m_reverse_edge_id_to_zipped_index_map.end());
  assert(iter->second < segment_data->nodes_size());
  return iter->second;
}*/

SegmentWeight CompressedEdgeContainer::clip_weight(const SegmentWeight weight) {
  if (weight >= INVALID_SEGMENT_WEIGHT) {
    clipped_weights++;
    return MAX_SEGMENT_WEIGHT;
  }
  return weight;
}

SegmentDuration
CompressedEdgeContainer::clip_duration(const SegmentDuration duration) {
  if (duration > INVALID_SEGMENT_DURATION) {
    clipped_weights++;
    return MAX_SEGMENT_DURATION;
  }
  return duration;
}

void CompressedEdgeContainer::compress_edge(
    const EdgeId surviving_edge_id, const EdgeId removed_edge_id,
    const NodeId via_node_id, const NodeId target_node,
    const EdgeWeight surviving_edge_weight,
    const EdgeWeight removed_edge_weight,
    const EdgeDuration surviving_edge_duration,
    const EdgeDuration removed_edge_duration,
    const EdgeWeight node_weight_penalty = INVALID_EDGE_WEIGHT,
    const EdgeDuration node_duration_penalty = MAXIMAL_EDGE_DURATION) {
  assert(surviving_edge_id != SPECIAL_EDGEID);
  assert(removed_edge_id != SPECIAL_EDGEID);
  assert(via_node_id != SPECIAL_NODEID);
  assert(target_node != SPECIAL_NODEID);
  assert(surviving_edge_weight != INVALID_SEGMENT_WEIGHT);
  assert(removed_edge_weight != INVALID_SEGMENT_WEIGHT);

  if (!has_entry_for_id(surviving_edge_id)) {
    if (m_free_list.size() == 0)
      increase_free_list();

    assert(!m_free_list.empty());

    m_edge_id_to_list_index_map[surviving_edge_id] = m_free_list.back();
    m_free_list.pop_back();
  }

  const auto iter = m_edge_id_to_list_index_map.find(surviving_edge_id);
  assert(iter != m_edge_id_to_list_index_map.end());

  const unsigned surviving_edge_bucket_id = iter->second;
  assert(surviving_edge_bucket_id == get_position_for_id(surviving_edge_id));
  assert(surviving_edge_bucket_id < m_compressed_oneway_geometries.size());

  std::vector<OneWayCompressedEdge> &surviving_edge_bucket_list =
      m_compressed_oneway_geometries[surviving_edge_bucket_id];

  if (surviving_edge_bucket_list.empty()) {
    surviving_edge_bucket_list.emplace_back(
        OneWayCompressedEdge{via_node_id, clip_weight(surviving_edge_weight),
                             clip_duration(surviving_edge_duration)});
  }

  assert(surviving_edge_bucket_list.size() > 0);
  assert(!surviving_edge_bucket_list.empty());

  if (node_weight_penalty != INVALID_EDGE_WEIGHT and
      node_duration_penalty != MAXIMAL_EDGE_DURATION) {
    surviving_edge_bucket_list.emplace_back(
        OneWayCompressedEdge{via_node_id, clip_weight(node_weight_penalty),
                             clip_duration(node_duration_penalty)});
  }

  if (has_entry_for_id(removed_edge_id)) {
    const unsigned list_to_remove_index = get_position_for_id(removed_edge_id);
    assert(list_to_remove_index < m_compressed_oneway_geometries.size());

    std::vector<OneWayCompressedEdge> &removed_edge_bucket_list =
        m_compressed_oneway_geometries[list_to_remove_index];
    surviving_edge_bucket_list.insert(surviving_edge_bucket_list.end(),
                                      removed_edge_bucket_list.begin(),
                                      removed_edge_bucket_list.end());

    m_edge_id_to_list_index_map.erase(removed_edge_id);
    assert(m_edge_id_to_list_index_map.end() ==
           m_edge_id_to_list_index_map.find(removed_edge_id));

    removed_edge_bucket_list.clear();
    assert(removed_edge_bucket_list.size() == 0);

    m_free_list.emplace_back(list_to_remove_index);
    assert(m_free_list.back() == list_to_remove_index);
  } else {
    surviving_edge_bucket_list.emplace_back(
        OneWayCompressedEdge{target_node, clip_weight(removed_edge_weight),
                             clip_duration(removed_edge_duration)});
  }
}

void CompressedEdgeContainer::add_uncompressed_edge(
    const EdgeId edge_id, const NodeId node_id, const SegmentWeight weight,
    const SegmentDuration duration) {
  assert(edge_id != SPECIAL_EDGEID);
  assert(node_id != SPECIAL_NODEID);
  assert(weight != INVALID_EDGE_WEIGHT);

  if (!has_entry_for_id(edge_id)) {
    if (m_free_list.size() == 0)
      increase_free_list();

    assert(!m_free_list.empty());
    m_edge_id_to_list_index_map[edge_id] = m_free_list.back();
    m_free_list.pop_back();
  }

  const auto iter = m_edge_id_to_list_index_map.find(edge_id);
  assert(iter != m_edge_id_to_list_index_map.end());

  const unsigned edge_bucket_id = iter->second;
  assert(edge_bucket_id == get_position_for_id(edge_id));
  assert(edge_bucket_id < m_compressed_oneway_geometries.size());

  std::vector<OneWayCompressedEdge> &edge_bucket_list =
      m_compressed_oneway_geometries[edge_bucket_id];

  if (edge_bucket_list.empty()) {
    edge_bucket_list.emplace_back(OneWayCompressedEdge{
        node_id, clip_weight(weight), clip_duration(duration)});
  }
}

void CompressedEdgeContainer::initialize_both_way_vector() {
  segment_data = std::make_unique<SegmentDataContainer>();
  segment_data->reserve(m_compressed_oneway_geometries.size());
}

unsigned CompressedEdgeContainer::zip_edges(const EdgeId forward_edge_id,
                                            const EdgeId reverse_edge_id) {
  if (!segment_data)
    initialize_both_way_vector();

  const auto &forward_bucket = get_bucket_reference(forward_edge_id);
  const auto &reverse_bucket = get_bucket_reference(reverse_edge_id);
  assert(forward_bucket.size() == reverse_bucket.size());

  const unsigned zipped_geometry_id = segment_data->index_size();
  m_forward_edge_id_to_zipped_index_map[forward_edge_id] = zipped_geometry_id;
  m_reverse_edge_id_to_zipped_index_map[reverse_edge_id] = zipped_geometry_id;

  const auto &first_node = reverse_bucket.back();
  constexpr DataSourceId LUA_SOURCE = 0;

  segment_data->emplace_back(segment_data->nodes_size(), first_node.node_id,
                             INVALID_SEGMENT_WEIGHT, first_node.weight,
                             INVALID_SEGMENT_DURATION, first_node.duration,
                             LUA_SOURCE, LUA_SOURCE);

  for (std::size_t index = 0; index < forward_bucket.size() - 1; ++index) {
    const auto &fwd_node = forward_bucket.at(index);
    const auto &rev_node = reverse_bucket.at(reverse_bucket.size() - 2 - index);
    assert(fwd_node.node_id == rev_node.node_id);

    segment_data->emplace_back(fwd_node.node_id, fwd_node.weight,
                               rev_node.weight, fwd_node.duration,
                               rev_node.duration, LUA_SOURCE, LUA_SOURCE);
  }

  const auto &last_node = forward_bucket.back();
  segment_data->emplace_back(last_node.node_id, last_node.weight,
                             INVALID_SEGMENT_WEIGHT, last_node.duration,
                             INVALID_SEGMENT_DURATION, LUA_SOURCE, LUA_SOURCE);

  return zipped_geometry_id;
}

void CompressedEdgeContainer::show_statistics() const {
  const uint64_t compressed_edges = m_compressed_oneway_geometries.size();
  assert(compressed_edges % 2 == 0);
  assert(compressed_edges + m_free_list.size() > 0);

  uint64_t compressed_geometries = 0, longest_chain_length = 0;

  for (const auto &current_vector : m_compressed_oneway_geometries) {
    compressed_geometries += current_vector.size();
    longest_chain_length =
        std::max(longest_chain_length, (uint64_t)current_vector.size());
  }

  if (clipped_weights > 0)
    fmt::format("Clipped {} segment weight to {}", clipped_weights,
                INVALID_SEGMENT_WEIGHT - 1);

  if (clipped_durations > 0)
    fmt::format("Clipped {} segment duration to {}", clipped_durations,
                INVALID_SEGMENT_DURATION - 1);

  fmt::format(
      "Geometry successfully removed: \n compressed edges: {}\n compressed "
      "geometries: {}\n longest chain length: {}\n cmpr ratio: {}\n avg chain "
      "length: {}",
      compressed_edges, compressed_geometries, longest_chain_length,
      (float)compressed_edges / std::max(compressed_geometries, (uint64_t)1),
      (float)(compressed_geometries) / std::max((uint64_t)1, compressed_edges));
}

const CompressedEdgeContainer::OnewayEdgeBucket &
CompressedEdgeContainer::get_bucket_reference(const EdgeId edge_id) const {
  const std::size_t index = m_edge_id_to_list_index_map.at(edge_id);
  return m_compressed_oneway_geometries.at(index);
}

bool CompressedEdgeContainer::is_trivial(const EdgeId edge_id) const {
  return get_bucket_reference(edge_id).size() == 1;
}

NodeId
CompressedEdgeContainer::get_first_edge_target_id(const EdgeId edge_id) const {
  const auto &bucket = get_bucket_reference(edge_id);
  assert(bucket.size() > 0);
  return bucket.front().node_id;
}

NodeId
CompressedEdgeContainer::get_last_edge_source_id(const EdgeId edge_id) const {
  const auto &bucket = get_bucket_reference(edge_id);
  assert(bucket.size() > 1);
  return bucket[bucket.size() - 2].node_id;
}

NodeId
CompressedEdgeContainer::get_last_edge_target_id(const EdgeId edge_id) const {
  const auto &bucket = get_bucket_reference(edge_id);
  assert(bucket.size() > 0);
  return bucket.back().node_id;
}

std::unique_ptr<SegmentDataContainer>
CompressedEdgeContainer::to_segment_data() {
  segment_data->push_back_index(segment_data->nodes_size());
  return std::move(segment_data);
}
} // namespace extractor
} // namespace osram
