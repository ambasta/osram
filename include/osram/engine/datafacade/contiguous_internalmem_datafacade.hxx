#ifndef OSRAM_ENGINE_DATAFACADE_CONTIGUOUS_INTERNALMEM_DATAFACADE
#define OSRAM_ENGINE_DATAFACADE_CONTIGUOUS_INTERNALMEM_DATAFACADE

#include <osram/engine/algorithm.hxx>
#include <osram/engine/datafacade/algorithm_datafacade.hxx>
#include <osram/engine/datafacade/contiguous_block_allocator.hxx>
#include <osram/engine/datafacade/datafacade_base.hxx>
#include <osram/extractor/profile_properties.hxx>

namespace osram {
namespace engine {
namespace datafacade {
template <typename T> class ContiguousInternalMemoryAlgorithmDataFacade;

template <>
class ContiguousInternalMemoryAlgorithmDataFacade<CH>
    : public datafacade::AlgorithmDataFacade<CH> {};

class ContiguousInternalMemoryDataFacadeBase : public BaseDataFacade {
private:
  extractor::ProfileProperties *m_profile_properties;
  extractor::ClassData exclude_mask;
  extractor::PackedOSMIDsView m_osmnodeid_list;
  std::vector<util::Coordinate> m_coordinate_list;
  std::vector<std::uint32_t> m_lane_description_offsets;
  std::vector<extractor::TurnLaneType::Mask> m_lane_description_masks;
  std::vector<TurnPenalty> m_turn_weight_penalties, m_turn_duration_penalties;
  extractor::SegmentDataView segment_data;
  extractor::EdgeBasedNodeDataView edge_based_node_data;
  guidance::TurnDataView turn_data;
  StaticRtree<BaseDataFacade::RTreeLeaf, storage::Ownership::View>
      m_static_rtree;
  std::unique_ptr<GeoSpatialQuery<
      StaticRtree<BaseDataFacade::RTreeLeaf, storage::Ownership::View>,
      BaseDataFacade>>
      m_geospatial_query;
  std::uint32_t m_checksum;

public:
  std::shared_ptr<ContiguousBlockAllocator> allocator;
  void initialize_internal_pointers(const storage::SharedDataIndex &index,
                                    std::string_view metric_name,
                                    const std::size_t exclude_index) {
    (void)metric_name;

    m_profile_properties =
        index.get_block_ptr<extractor::ProfileProperties>("/common/properties");
    exclude_mask = m_profile_properties->excludable_classes[exclude_index];
    m_checksum =
        *index.get_block_ptr<std::uint32_t>("/common/connectivity_checksum");

    std::tie(m_coordinate_list, m_osmnodeid_list) =
        make_nbn_data_view(index, "/common/nbn_data");
    m_static_rtree = make_search_tree(index, "/common/rtree");
    m_geospatial_query.reset(
        new SharedGeoSpatialQuery(m_static_rtree, m_coordinate_list, *this));

    edge_based_node_data = make_ebn_data_view(index, "/common/ebg_node_data");

    turn_data = make_turn_data_view(index, "/common/turn_data");

    m_name_table = make_name_table_view(index, "/common/names");

    std::tie(m_lane_description_offsets, m_lane_description_masks) =
        make_turn_lane_description_views(index, "/common/turn_lanes");
    m_lane_tupel_id_pairs = make_lane_data_view(index, "/common/turn_lanes");

    m_turn_weight_penalties =
        make_turn_weight_view(index, "/common/turn_penalty");
    m_turn_duration_penalties =
        make_turn_duration_view(index, "/common/turn_penalty");

    segment_data = make_segment_data_view(index, "/common/segment_data");

    m_datasources =
        index.GetBlockPtr<extractor::Datasources>("/common/data_sources_names");

    intersection_bearings_view =
        make_intersection_bearings_view(index, "/common/intersection_bearings");

    m_entry_class_table =
        make_entry_classes_view(index, "/common/entry_classes");

    std::tie(m_maneuver_overrides, m_maneuver_override_node_sequences) =
        make_maneuver_overrides_views(index, "/common/maneuver_overrides");
  }
  ContiguousInternalMemoryDataFacadeBase(
      std::shared_ptr<ContiguousBlockAllocator> allocator,
      const std::string &metric_name, const std::size_t exclude_index)
      : allocator(std::move(allocator)) {
    initialize_internal_pointers(allocator->get_index(), metric_name,
                                 exclude_index);
  }
  std::uint32_t get_checksum() const override final { return m_checksum; }
};

template <typename T> class ContiguousInternalMemoryDataFacade;

template <>
class ContiguousInternalMemoryDataFacade<CH>
    : public ContiguousInternalMemoryDataFacadeBase,
      public ContiguousInternalMemoryAlgorithmDataFacade<CH> {};

template <>
class ContiguousInternalMemoryAlgorithmDataFacade<MLD>
    : public AlgorithmDataFacade<MLD> {};

template <>
class ContiguousInternalMemoryDataFacade<MLD> final
    : public ContiguousInternalMemoryDataFacadeBase,
      public ContiguousInternalMemoryAlgorithmDataFacade<MLD> {};

} // namespace datafacade
} // namespace engine
} // namespace osram
#endif
