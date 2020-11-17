#ifndef OSRAM_EXTRACTOR_COMPRESSED_EDGE_CONTAINER
#define OSRAM_EXTRACTOR_COMPRESSED_EDGE_CONTAINER

#include <memory>
#include <osram/extractor/segment_data_container.hxx>
#include <osram/util/typedefs.hxx>
#include <unordered_map>
#include <vector>

namespace osram {
namespace extractor {
class CompressedEdgeContainer {
public:
  struct OneWayCompressedEdge {
    NodeId node_id;
    SegmentWeight weight;
    SegmentDuration duration;
  };

  using OnewayEdgeBucket = std::vector<OneWayCompressedEdge>;

private:
  int free_lis_max = 0;

  std::atomic_size_t clipped_weights{0}, clipped_durations{0};

  std::vector<OnewayEdgeBucket> m_compressed_oneway_geometries;
  std::vector<unsigned> m_free_list;

  std::unordered_map<EdgeId, unsigned> m_edge_id_to_list_index_map,
      m_forward_edge_id_to_zipped_index_map,
      m_reverse_edge_id_to_zipped_index_map;
  std::unique_ptr<SegmentDataContainer> segment_data;

  SegmentWeight clip_weight(const SegmentWeight);
  SegmentDuration clip_duration(const SegmentDuration);

  void increase_free_list();

public:
  CompressedEdgeContainer();

  void compress_edge(const EdgeId, const EdgeId, const NodeId, const NodeId,
                     const EdgeWeight, const EdgeWeight, const EdgeDuration,
                     const EdgeDuration, const EdgeWeight, const EdgeDuration);

  void add_uncompressed_edge(const EdgeId, const NodeId, const SegmentWeight,
                             const SegmentDuration);

  void initialize_both_way_vector();

  unsigned zip_edges(const EdgeId, const EdgeId);

  bool has_entry_for_id(const EdgeId) const;

  bool has_zipped_entry_for_forward_id(const EdgeId) const;

  bool has_zipped_entry_for_reverse_id(const EdgeId) const;

  void show_statistics() const;

  unsigned get_position_for_id(const EdgeId) const;

  unsigned get_zipped_position_for_forward_id(const EdgeId) const;

  unsigned get_zipped_position_for_reverse_id(const EdgeId) const;

  const OnewayEdgeBucket &get_bucket_reference(const EdgeId) const;

  bool is_trivial(const EdgeId) const;

  NodeId get_first_edge_target_id(const EdgeId) const;

  NodeId get_last_edge_target_id(const EdgeId) const;

  NodeId get_last_edge_source_id(const EdgeId) const;

  std::unique_ptr<SegmentDataContainer> to_segment_data();
};
} // namespace extractor
} // namespace osram
#endif
