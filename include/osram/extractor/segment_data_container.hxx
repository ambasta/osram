#ifndef OSRAM_EXTRACTOR_SEGMENT_DATA_CONTAINER
#define OSRAM_EXTRACTOR_SEGMENT_DATA_CONTAINER
#include <osram/storage/shared_memory_ownership.hxx>
#include <osram/storage/tar.hxx>
#include <osram/storage/tar_fwd.hxx>
#include <osram/util/typedefs.hxx>
#include <string_view>
#include <vector>

namespace osram {
namespace extractor {
namespace detail {
template <storage::Ownership O> class SegmentDataContainerImpl;
}
namespace serialization {
template <storage::Ownership O>
inline void read(storage::tar::FileReader &reader, std::string_view name,
                 detail::SegmentDataContainerImpl<O> &segment_data);

template <storage::Ownership O>
inline void write(storage::tar::FileWriter &writer, std::string_view name,
                  detail::SegmentDataContainerImpl<O> &segment_data);
} // namespace serialization
namespace detail {
template <storage::Ownership O> class SegmentDataContainerImpl {
public:
  using DirectionalGeometryId = std::uint32_t;
  using SegmentOffset = std::uint32_t;
  using SegmentNodeVector = std::vector<NodeId>;
  using SegmentWeightVector = std::vector<SegmentWeight>;
  using SegmentDurationVector = std::vector<SegmentDuration>;
  using SegmentDataSourceVector = std::vector<DataSourceId>;

private:
  std::vector<std::uint32_t> index;
  SegmentNodeVector nodes;
  SegmentWeightVector fwd_weights, rev_weights;
  SegmentDurationVector fwd_durations, rev_durations;
  SegmentDataSourceVector fwd_datasources, rev_datasources;

public:
  SegmentDataContainerImpl() = default;

  SegmentDataContainerImpl(std::vector<std::uint32_t> index,
                           SegmentNodeVector &nodes,
                           SegmentWeightVector fwd_weights,
                           SegmentWeightVector rev_weights,
                           SegmentDurationVector fwd_durations,
                           SegmentDurationVector rev_durations,
                           SegmentDataSourceVector fwd_datasources,
                           SegmentDataSourceVector rev_datasources)
      : index(index), nodes(nodes), fwd_weights(fwd_weights),
        rev_weights(rev_weights), fwd_durations(fwd_durations),
        rev_durations(rev_durations), fwd_datasources(fwd_datasources),
        rev_datasources(rev_datasources) {}

  void reserve(std::size_t sz) {
    index.reserve(sz);
    nodes.reserve(sz);
    fwd_weights.reserve(sz);
    rev_weights.reserve(sz);
    fwd_durations.reserve(sz);
    rev_durations.reserve(sz);
    fwd_datasources.reserve(sz);
    rev_datasources.reserve(sz);
  }

  auto get_forward_geometry(const DirectionalGeometryId id) {
    const auto begin = nodes.begin() + index[id];
    const auto end = nodes.begin() + index[id + 1];
    return std::make_pair(begin, end);
  }

  auto get_reverse_geometry(const DirectionalGeometryId id) {
    auto resp = get_forward_geometry(id);
    return std::make_pair(resp.second, resp.first);
  }

  auto get_forward_durations(const DirectionalGeometryId id) {
    const auto begin = fwd_durations.begin() + index[id] + 1;
    const auto end = fwd_durations.begin() + index[id + 1];
    return std::make_pair(begin, end);
  }

  auto get_reverse_durations(const DirectionalGeometryId id) {
    const auto begin = rev_durations.begin() + index[id];
    const auto end = rev_durations.begin() + index[id + 1] - 1;
    return std::make_pair(begin, end);
  }

  std::size_t nodes_size() const { return nodes.size(); }

  std::size_t index_size() const { return index.size(); }

  void emplace_index(const std::size_t sz) { index.emplace_back(sz); }

  void push_back_index(const std::size_t sz) { index.push_back(sz); }

  void emplace_nodes(const NodeId node_id) { nodes.emplace_back(node_id); }

  void emplace_back(NodeId node_id, SegmentWeight fwd_weight,
                    SegmentWeight rev_weight, SegmentDuration fwd_duration,
                    SegmentDuration rev_duration, DataSourceId fwd_datasource,
                    DataSourceId rev_datasource) {
    nodes.emplace_back(node_id);
    fwd_weights.emplace_back(fwd_weight);
    rev_weights.emplace_back(rev_weight);
    fwd_durations.emplace_back(fwd_duration);
    rev_durations.emplace_back(rev_duration);
    fwd_datasources.emplace_back(fwd_datasource);
    rev_datasources.emplace_back(rev_datasource);
  }

  void emplace_back(std::size_t sz, NodeId node_id, SegmentWeight fwd_weight,
                    SegmentWeight rev_weight, SegmentDuration fwd_duration,
                    SegmentDuration rev_duration, DataSourceId fwd_datasource,
                    DataSourceId rev_datasource) {
    index.emplace_back(sz);
    nodes.emplace_back(node_id);
    fwd_weights.emplace_back(fwd_weight);
    rev_weights.emplace_back(rev_weight);
    fwd_durations.emplace_back(fwd_duration);
    rev_durations.emplace_back(rev_duration);
    fwd_datasources.emplace_back(fwd_datasource);
    rev_datasources.emplace_back(rev_datasource);
  }

  auto get_number_of_geometries() const { return index.size() - 1; }
  auto get_number_of_segments() const { return fwd_weights.size(); }

  /*friend void
  serialization::read<O>(storage::tar::FileReader &reader,
                         std::string_view name,
                         detail::SegmentDataContainerImpl<O> &segment_data);

  friend void serialization::write<O>(
      storage::tar::FileWriter &writer, std::string_view name,
      const detail::SegmentDataContainerImpl<O> &segment_data); */
};
} // namespace detail

using SegmentDataView =
    detail::SegmentDataContainerImpl<storage::Ownership::View>;
using SegmentDataContainer =
    detail::SegmentDataContainerImpl<storage::Ownership::Container>;
} // namespace extractor
} // namespace osram
#endif
