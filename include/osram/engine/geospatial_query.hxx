#ifndef OSRAM_ENGINE_GEOSPATIAL_QUERY
#define OSRAM_ENGINE_GEOSPATIAL_QUERY

#include <algorithm>
#include <cmath>
#include <iterator>
#include <osram/engine/approach.hxx>
#include <osram/engine/phantom_node.hxx>

#include <osram/util/bearing.hxx>
#include <osram/util/coordinate.hxx>
#include <osram/util/coordinate_calculation.hxx>
#include <osram/util/rectangle.hxx>
#include <osram/util/typedefs.hxx>
#include <osram/util/web_mercator.hxx>

#include <numeric>
#include <utility>
#include <vector>

namespace osram {
namespace engine {
inline std::pair<bool, bool> bool_pair_and(const std::pair<bool, bool> &,
                                           const std::pair<bool, bool> &);

template <typename T, typename U> class GeospatialQuery {
  using EdgeData = typename T::EdgeData;
  using CoordinateList = typename T::CoordinateList;
  using CandidateSegment = typename T::CandidateSegment;

private:
  const T &rtree;
  const CoordinateList &coordinates;
  U &datafacade;

  std::vector<PhantomNodeWithDistance>
  make_phantom_nodes(const util::Coordinate coordinate,
                     const std::vector<EdgeData> &results) const {
    std::vector<PhantomNodeWithDistance> distance_and_phantoms(results.size());
    std::transform(results.begin(), results.end(),
                   distance_and_phantoms.begin(),
                   [this, &coordinate](const EdgeData &data) {
                     return make_phantom_nodes(coordinate, data);
                   });
    return distance_and_phantoms;
  }

  PhantomNodeWithDistance make_phantom_nodes(const util::Coordinate coordinate,
                                             const EdgeData &data) const {
    util::Coordinate point_on_segment;
    double ratio;
    const auto current_perpendicular_distance =
        util::coordinate_calculation::perpendicular_distance(
            coordinates[data.u], coordinates[data.v], coordinate,
            point_on_segment, ratio);

    assert(data.forward_segment_id.enabled or data.reverse_segment_id.enabled);
    assert(!data.reverse_segment_id.enabled or
           datafacade.get_geometry_index(data.forward_segment_id.id).id ==
               datafacade.get_geometry_index(data.reverse_segment_id.id).id);

    const auto geometry_id =
        datafacade.get_geometry_index(data.forward_segment_id.id).id;
    const auto component_id =
        datafacade.get_component_id(data.forward_segment_id.id);

    const auto forward_weights =
        datafacade.get_uncompressed_forward_weights(geometry_id);
    const auto reverse_weights =
        datafacade.get_uncompressed_reverse_weights(geometry_id);

    const auto forward_durations =
        datafacade.get_uncompressed_forward_durations(geometry_id);
    const auto reverse_durations =
        datafacade.get_uncompressed_reverse_durations(geometry_id);

    const auto forward_geometry =
        datafacade.get_uncompressed_forward_geometry(geometry_id);

    const auto forward_weight_offset = std::accumulate(
        forward_weights.begin(),
        forward_weights.begin() + data.fwd_segment_position, EdgeWeight{0});

    const auto forward_duration_offset = std::accumulate(
        forward_durations.begin(),
        forward_durations.begin() + data.fwd_segment_position, EdgeDuration{0});

    EdgeDistance forward_distance_offset = 0;

    for (auto current = forward_geometry.begin();
         current < forward_geometry.begin() + data.fwd_segment_position;
         ++current) {
      forward_distance_offset +=
          util::coordinate_calculation::fcc_approximate_distance(
              datafacade.get_coordinate_of_node(*current),
              datafacade.get_coordinate_of_node(*std::next(current)));
    }

    assert(data.fwd_segment_position <
           std::distance(forward_durations.begin(), forward_durations.end()));

    EdgeWeight forward_weight = forward_weights[data.fwd_segment_position];
    EdgeDuration forward_duration =
        forward_durations[data.fwd_segment_position];
    EdgeDistance forward_distance =
        util::coordinate_calculation::fcc_approximate_distance(
            datafacade.get_coordinate_of_node(
                forward_geometry(data.fwd_segment_position)),
            point_on_segment);

    const auto reverse_weight_offset = std::accumulate(
        reverse_weights.begin(),
        reverse_weights.end() - data.fwd_segment_position - 1, EdgeWeight{0});

    const auto reverse_duration_offset =
        std::accumulate(reverse_durations.begin(),
                        reverse_durations.end() - data.fwd_segment_position - 1,
                        EdgeDuration{0});

    EdgeDistance reverse_distance_offset = 0;

    for (auto current =
             forward_geometry.begin() + data.fwd_segment_position + 1;
         current != std::prev(forward_geometry.end()); ++current) {
      reverse_distance_offset +=
          util::coordinate_calculation::fcc_approximate_distance(
              datafacade.get_coordinate_of_node(*current),
              datafacade.get_coordinate_of_node(*std::next(current)));
    }

    EdgeWeight reverse_weight =
        reverse_weights[reverse_weights.size() - data.fwd_segment_position - 1];
    EdgeDuration reverse_duration =
        reverse_durations[reverse_durations.size() - data.fwd_segment_position -
                          1];
    EdgeDistance reverse_distance =
        util::coordinate_calculation::fcc_approximate_distance(
            point_on_segment,
            datafacade.get_coordinate_of_node(
                forward_geometry(data.fwd_segment_position + 1)));

    ratio = std::min(1.0, std::max(0.0, ratio));

    if (data.forward_segment_id.id != SPECIAL_SEGMENTID) {
      forward_weight = static_cast<EdgeWeight>(forward_weight * ratio);
      forward_duration = static_cast<EdgeDuration>(forward_duration * ratio);
    }

    if (data.reverse_segment_id.id != SPECIAL_SEGMENTID) {
      reverse_weight -= static_cast<EdgeWeight>(reverse_weight * ratio);
      reverse_duration -= static_cast<EdgeDuration>(reverse_duration * ratio);
    }

    auto are_segments_valid = [](auto first, auto last) -> bool {
      return std::find(first, last, INVALID_SEGMENT_WEIGHT) == last;
    };

    bool is_forward_valid_source =
        are_segments_valid(forward_weights.begin(), forward_weights.end());

    bool is_forward_valid_target = are_segments_valid(
        forward_weights.begin(),
        forward_weights.begin() + data.fwd_segment_position + 1);

    bool is_reverse_valid_source =
        are_segments_valid(reverse_weights.begin(), reverse_weights.end());

    bool is_reverse_valid_target =
        are_segments_valid(reverse_weights.begin(),
                           reverse_weights.end() - data.fwd_segment_position);

    auto transformed = PhantomNodeWithDistance{
        PhantomNode{
            data,
            component_id,
            forward_weight,
            reverse_weight,
            forward_weight_offset,
            reverse_weight_offset,
            forward_distance,
            reverse_distance,
            forward_distance_offset,
            reverse_distance_offset,
            forward_duration,
            reverse_duration,
            forward_duration_offset,
            reverse_duration_offset,
            is_forward_valid_source,
            is_forward_valid_target,
            is_reverse_valid_source,
            is_reverse_valid_target,
            point_on_segment,
            coordinate,
            static_cast<unsigned short>(util::coordinate_calculation::bearing(
                coordinates[data.u], coordinates[data.v]))},
        current_perpendicular_distance};
    return transformed;
  }

  bool check_segment_distance(const util::Coordinate coordinate,
                              const CandidateSegment &segment,
                              const double max_distance) const {
    assert(segment.data.forward_segment_id.id != SPECIAL_SEGMENTID or
           !segment.data.forward_segment_id.enabled);
    assert(segment.data.reverse_segment_id.id != SPECIAL_SEGMENTID or
           !segment.data.reverse_segment_id.enabled);

    util::Coordinate wgs84_coordinate =
        util::web_mercator::to_wgs84(segment.fixed_projected_coordinate);
    return util::coordinate_calculation::haversine_distance(
               coordinate, wgs84_coordinate) > max_distance;
  }

  std::pair<bool, bool>
  check_segment_exclude(const CandidateSegment &segment) const {
    std::pair<bool, bool> valid{true, true};

    if (segment.data.forward_segment_id.enabled and
        datafacade.exclude_node(segment.data.forward_segment_id.id))
      valid.first = false;

    if (segment.data.reverse_segment_id.enabled and
        datafacade.exclude_node(segment.data.reverse_segment_id.id))
      valid.second = false;
    return valid;
  }

  std::pair<bool, bool>
  check_segment_bearing(const CandidateSegment &segment,
                        const int filter_bearing,
                        const int filter_bearing_range) const {
    assert(segment.data.forward_segment_id.id != SPECIAL_SEGMENTID or
           !segment.data.forward_segment_id.enabled);
    assert(segment.data.reverse_segment_id.id != SPECIAL_SEGMENTID or
           !segment.data.reverse_segment_id.enabled);

    const double forward_edge_bearing = util::coordinate_calculation::bearing(
        coordinates[segment.data.u], coordinates[segment.data.v]);
    const double backward_edge_bearing = forward_edge_bearing + 180 > 360
                                             ? forward_edge_bearing - 180
                                             : forward_edge_bearing + 180;
    const bool forward_bearing_valid =
        util::bearing::check_inbounds(std::round(forward_edge_bearing),
                                      filter_bearing, filter_bearing_range) and
        segment.data.forward_segment_id.enabled;
    const bool backward_bearing_valid =
        util::bearing::check_inbounds(std::round(backward_edge_bearing),
                                      filter_bearing, filter_bearing_range) and
        segment.data.reverse_segment_id.enabled;

    return std::make_pair(forward_bearing_valid, backward_bearing_valid);
  }

  std::pair<bool, bool> has_valid_edge(const CandidateSegment &segment,
                                       const bool use_all_edges = false) const {
    bool forward_edge_valid = false, reverse_edge_valid = false;
    const auto &data = segment.data;

    assert(data.forward_segment_id.enabled);
    assert(data.forward_segment_id.id != SPECIAL_NODEID);

    const auto geometry_id =
        datafacade.get_geometry_index(data.forward_segment_id.id).id;

    const auto forward_weights =
        datafacade.get_uncompressed_forward_weights(geometry_id);
    const auto reverse_weights =
        datafacade.get_uncompressed_reverse_weights(geometry_id);

    if (forward_weights[data.fwd_segment_position] != INVALID_SEGMENT_WEIGHT)
      forward_edge_valid = data.forward_segment_id.enabled;

    if (reverse_weights[reverse_weights.size() - data.fwd_segment_position -
                        1] != INVALID_SEGMENT_WEIGHT)
      reverse_edge_valid = data.reverse_segment_id.enabled;

    forward_edge_valid =
        forward_edge_valid and (data.is_startpoint or use_all_edges);
    reverse_edge_valid =
        reverse_edge_valid and (data.is_startpoint or use_all_edges);
    return std::make_pair(forward_edge_valid, reverse_edge_valid);
  }

  bool is_tiny_component(const CandidateSegment &segment) const {
    const auto &data = segment.data;
    assert(data.forward_segment_id.enabled);
    assert(data.forward_segment_id.id != SPECIAL_NODEID);

    return datafacade.get_component_id(data.forward_segment_id.id).is_tiny;
  }

  std::pair<bool, bool> check_approach(const util::Coordinate &coordinate,
                                       const CandidateSegment &segment,
                                       const Approach approach) const {

    bool is_one_way_segment = !(segment.data.forward_segment_id.enabled and
                                segment.data.reverse_segment_id.enabled);

    if (!is_one_way_segment and approach == Approach::CURB) {
      bool input_coordinate_is_at_right = !util::coordinate_calculation::is_ccw(
          coordinates[segment.data.u], coordinates[segment.data.v], coordinate);

      if (datafacade.is_left_hand_driving(segment.data.forward_segment_id.id))
        input_coordinate_is_at_right = !input_coordinate_is_at_right;

      return std::make_pair(input_coordinate_is_at_right,
                            (!input_coordinate_is_at_right));
    }
    return std::make_pair(true, true);
  }

public:
  GeospatialQuery(T &rtree, const CoordinateList &coordinates, U &datafacade)
      : rtree(rtree), coordinates(coordinates), datafacade(datafacade) {}

  std::vector<EdgeData> search(const util::RectangleInt2D &bounding_box) {
    return rtree.search_in_box(bounding_box);
  }

  std::vector<PhantomNodeWithDistance> nearest_phantom_nodes_in_range(
      const util::Coordinate coordinate, const double max_distance,
      const Approach approach, const bool use_all_edges) const {
    auto results = rtree.nearest(
        coordinate,
        [this, approach, &coordinate,
         use_all_edges](const CandidateSegment &segment) {
          return bool_pair_and(
              bool_pair_and(has_valid_edge(segment, use_all_edges),
                            check_segment_exclude(segment)),
              check_approach(coordinate, segment, approach));
        },
        [this, max_distance, coordinate](const std::size_t,
                                         const CandidateSegment &segment) {
          return check_segment_distance(coordinate, segment, max_distance);
        });
    return make_phantom_nodes(coordinate, results);
  }

  std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes(const util::Coordinate coordinate,
                        const unsigned max_results, const int bearing,
                        const int bearing_range,
                        const Approach approach) const {
    auto results = rtree.nearest(
        coordinate,
        [this, approach, &coordinate, bearing,
         bearing_range](const CandidateSegment &segment) {
          auto use_direction = bool_pair_and(
              check_segment_bearing(segment, bearing, bearing_range),
              bool_pair_and(has_valid_edge(segment),
                            check_segment_exclude(segment)));
          return bool_pair_and(use_direction,
                               check_approach(coordinate, segment, approach));
        },
        [max_results](const std::size_t num_results, const CandidateSegment &) {
          return num_results >= max_results;
        });
    return make_phantom_nodes(coordinate, results);
  }
};
} // namespace engine
} // namespace osram

#endif
