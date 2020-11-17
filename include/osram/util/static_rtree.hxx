#ifndef OSRAM_UTIL_STATIC_RTREE
#define OSRAM_UTIL_STATIC_RTREE

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <execution>
#include <iterator>
#include <limits>
#include <numeric>
#include <queue>

#include <boost/iostreams/device/mapped_file.hpp>
#include <range/v3/view/iota.hpp>

#include <osram/storage/shared_memory_ownership.hxx>

#include <osram/util/coordinate.hxx>
#include <osram/util/coordinate_calculation.hxx>
#include <osram/util/hilbert_value.hxx>
#include <osram/util/mmap_file.hxx>
#include <osram/util/rectangle.hxx>
#include <osram/util/vector_view.hxx>
#include <osram/util/web_mercator.hxx>

namespace osram {
namespace util {
template <class E, storage::Ownership O = storage::Ownership::Container,
          std::uint32_t BRANCHING_FACTOR = 64,
          std::uint32_t LEAF_PAGE_SIZE = 4096>
class StaticRTree {
  template <typename T> using Vector = ViewOrVector<T, O>;

public:
  struct TreeIndex;
  struct TreeNode;

private:
  Vector<TreeNode> m_search_tree;
  util::vector_view<const Coordinate> m_coordinate_list;
  Vector<std::uint64_t> m_tree_level_starts;
  boost::iostreams::mapped_file_source m_objects_region;
  util::vector_view<const E> m_objects;

  struct WrappedInputElement {
    std::uint64_t hilbert_value;
    std::uint32_t original_index;

    explicit WrappedInputElement(const std::uint64_t hilbert_value,
                                 const std::uint32_t original_index)
        : hilbert_value(hilbert_value), original_index(original_index) {}

    WrappedInputElement() : hilbert_value(0), original_index(UINT_MAX) {}

    inline bool operator<(const WrappedInputElement &other) const {
      return hilbert_value < other.hilbert_value;
    }
  };

  struct QueryCandidate {
    std::uint64_t squared_min_dist;
    TreeIndex tree_index;
    std::uint32_t segment_index;
    Coordinate fixed_projected_coordinate;

    QueryCandidate(std::uint64_t squared_min_dist, TreeIndex tree_index)
        : squared_min_dist(squared_min_dist), tree_index(tree_index),
          segment_index(std::numeric_limits<std::uint32_t>::max()) {}

    QueryCandidate(std::uint64_t squared_min_dist, TreeIndex tree_index,
                   const std::uint32_t segment_index,
                   const Coordinate &fixed_projected_coordinate)
        : squared_min_dist(squared_min_dist), tree_index(tree_index),
          segment_index(segment_index),
          fixed_projected_coordinate(fixed_projected_coordinate) {}

    inline bool is_segment() const {
      return segment_index != std::numeric_limits<std::uint32_t>::max();
    }

    inline bool operator<(const QueryCandidate &other) const {
      return other.squared_min_dist < squared_min_dist;
    }
  };

  template <typename Q>
  void explore_leaf_node(const TreeIndex &leaf_id,
                         const Coordinate &projected_input_coordinate_fixed,
                         const FloatCoordinate &projected_input_coordinate,
                         Q &traversal_queue) const {
    assert(is_leaf(leaf_id));

    for (auto const index : child_indices(leaf_id)) {
      const auto &current_edge = m_objects[index];
      const auto projected_u =
          web_mercator::from_wgs84(m_coordinate_list[current_edge.u]);
      const auto projected_v =
          web_mercator::from_wgs84(m_coordinate_list[current_edge.v]);

      FloatCoordinate projected_nearest;
      std::tie(std::ignore, projected_nearest) =
          coordinate_calculation::project_point_on_segment(
              projected_u, projected_v, projected_input_coordinate);

      const auto squared_distance =
          coordinate_calculation::squared_euclidean_distance(
              projected_input_coordinate_fixed, projected_nearest);

      assert(0. <= squared_distance);
      assert(index < std::numeric_limits<std::uint32_t>::max());
      traversal_queue.push(QueryCandidate{squared_distance, leaf_id,
                                          static_cast<std::uint32_t>(index),
                                          Coordinate{projected_nearest}});
    }
  }

  template <class Q>
  void explore_tree_node(const TreeIndex &parent,
                         const Coordinate &fixed_projected_input_coordinate,
                         Q &traversal_queue) const {
    assert(!is_leaf(parent));

    for (const auto child_index : child_indices(parent)) {
      const auto &child = m_search_tree[child_index];
      const auto squared_lower_bound_to_element =
          child.minimum_bounding_rectangle.get_min_squared_dist(
              fixed_projected_input_coordinate);
      traversal_queue.push(QueryCandidate{
          squared_lower_bound_to_element,
          TreeIndex(parent.level + 1,
                    child_index - m_tree_level_starts[parent.level + 1])});
    }
  }

  std::uint64_t get_level_size(const std::size_t level) const {
    assert(m_tree_level_starts.size() > level + 1);
    assert(m_tree_level_starts[level + 1] >= m_tree_level_starts[level]);
    return m_tree_level_starts[level + 1] - m_tree_level_starts[level];
  }

  ranges::iota_view<unsigned long, unsigned long>
  child_indices(const TreeIndex &parent) const {
    if (is_leaf(parent)) {
      const std::uint64_t first_child_index = parent.offset * LEAF_NODE_SIZE;
      const std::uint64_t end_child_index =
          std::min(first_child_index + LEAF_NODE_SIZE,
                   static_cast<std::uint64_t>(m_objects.size()));
      assert(first_child_index < std::numeric_limits<std::uint32_t>::max());
      assert(end_child_index < std::numeric_limits<std::uint32_t>::max());
      assert(end_child_index <= m_objects.size());

      return ranges::views::iota(first_child_index, end_child_index);
    }
    const std::uint64_t first_child_index =
        m_tree_level_starts[parent.level + 1] +
        parent.offset * BRANCHING_FACTOR;
    const std::uint64_t end_child_index =
        std::min(first_child_index + BRANCHING_FACTOR,
                 m_tree_level_starts[parent.level + 1] +
                     get_level_size(parent.level + 1));

    assert(first_child_index < std::numeric_limits<std::uint32_t>::max());
    assert(end_child_index < std::numeric_limits<std::uint32_t>::max());
    assert(end_child_index <= m_search_tree.size());
    assert(end_child_index <= m_tree_level_starts[parent.level + 1] +
                                  get_level_size(parent.level + 1));

    return ranges::views::iota(first_child_index, end_child_index);
  }

  bool is_leaf(const TreeIndex &tree_index) const {
    assert(m_tree_level_starts.size() > 1);
    return tree_index.level == m_tree_level_starts.size() - 2;
  }

public:
  static_assert(LEAF_PAGE_SIZE >= sizeof(E), "page_size is too small");
  static_assert(((LEAF_PAGE_SIZE - 1) & LEAF_PAGE_SIZE) == 0,
                "Page size is not a power of 2");
  static constexpr std::uint32_t LEAF_NODE_SIZE = (LEAF_PAGE_SIZE / sizeof(E));

  struct CandidateSegment {
    Coordinate fixed_projected_coordinate;
    E data;
  };

  struct TreeIndex {
    std::uint32_t level, offset;

    TreeIndex() : level(0), offset(0) {}

    TreeIndex(std::uint32_t level, std::uint32_t offset)
        : level(level), offset(offset) {}
  };

  struct TreeNode {
    RectangleInt2D minimum_bounding_rectangle;
  };

public:
  StaticRTree() = default;

  StaticRTree(const StaticRTree &) = delete;

  StaticRTree &operator=(const StaticRTree &) = delete;

  StaticRTree(StaticRTree &&) = default;

  StaticRTree &operator=(StaticRTree &&) = default;

  explicit StaticRTree(const std::vector<E> &input_data_vector,
                       const Vector<Coordinate> &coordinate_list,
                       const std::filesystem::path &on_disk_filename)
      : m_coordinate_list(coordinate_list.data(), coordinate_list.size()) {
    const auto elem_count = input_data_vector.size();
    std::vector<WrappedInputElement> input_wrapper_vector(elem_count);

    std::for_each(
        std::execution::par, ranges::views::iota(0, (int)elem_count),
        [&input_data_vector, &input_wrapper_vector, this](int index) {
          WrappedInputElement &current_wrapper = input_wrapper_vector[index];
          current_wrapper.original_index = index;

          E const &current_element = input_data_vector[index];
          assert(current_element.u < m_coordinate_list.size());
          assert(current_element.v < m_coordinate_list.size());

          Coordinate current_centroid = coordinate_calculation::centroid(
              m_coordinate_list[current_element.u],
              m_coordinate_list[current_element.v]);
          current_centroid.latitude = FixedLatitude{static_cast<std::int32_t>(
              COORDINATE_PRECISION *
              web_mercator::lat_to_y(to_floating(current_centroid.latitude)))};
          current_wrapper.hilbert_value = get_hilbert_code(current_centroid);
        });

    std::sort(std::execution::par, input_wrapper_vector.begin(),
              input_wrapper_vector.end());

    {
      boost::iostreams::mapped_file out_objects_region;
      auto out_objects = mmap_file<E>(on_disk_filename, out_objects_region,
                                      input_data_vector.size() * sizeof(E));

      std::size_t wrapped_elem_index = 0;
      auto objects_iter = out_objects.begin();

      while (wrapped_elem_index < elem_count) {
        TreeNode current_node;

        for (std::uint32_t object_index = 0;
             object_index < LEAF_NODE_SIZE and wrapped_elem_index < elem_count;
             ++object_index, ++wrapped_elem_index) {
          const std::uint32_t input_object_index =
              input_wrapper_vector[wrapped_elem_index].original_index;
          const E &object = input_data_vector[input_object_index];

          *objects_iter++ = object;

          Coordinate projected_u{web_mercator::from_wgs84(
              Coordinate{m_coordinate_list[object.u]})};
          Coordinate projected_v{web_mercator::from_wgs84(
              Coordinate(m_coordinate_list[object.v]))};

          assert(std::abs(to_floating(projected_u.longitude)) <= 180);
          assert(std::abs(to_floating(projected_u.latitude)) <= 180);
          assert(std::abs(to_floating(projected_v.longitude)) <= 180);
          assert(std::abs(to_floating(projected_v.latitude)) <= 180);

          RectangleInt2D rectangle;
          rectangle.min_lon =
              std::min(rectangle.min_lon,
                       std::min(projected_u.longitude, projected_v.longitude));
          rectangle.max_lon =
              std::max(rectangle.max_lon,
                       std::max(projected_u.longitude, projected_v.longitude));
          rectangle.min_lat =
              std::min(rectangle.min_lat,
                       std::min(projected_u.latitude, projected_v.latitude));
          rectangle.max_lat =
              std::max(rectangle.max_lat,
                       std::max(projected_u.latitude, projected_v.latitude));

          assert(rectangle.is_valid());
          current_node.minimum_bounding_rectangle = rectangle;
        }
        m_search_tree.emplace_back(current_node);
      }
    }

    m_objects = mmap_file<E>(on_disk_filename, m_objects_region);
    std::uint32_t nodes_in_previous_level = m_search_tree.size();
    std::vector<std::uint64_t> tree_level_sizes;
    tree_level_sizes.push_back(nodes_in_previous_level);

    while (nodes_in_previous_level > 1) {
      auto previous_level_start_pos =
          m_search_tree.size() - nodes_in_previous_level;
      std::uint32_t nodes_in_current_level = std::ceil(
          static_cast<double>(nodes_in_previous_level) / BRANCHING_FACTOR);

      for (auto current_node_index :
           ranges::views::iota(0, nodes_in_current_level)) {
        TreeNode parent_node;
        auto first_child_index =
            current_node_index * BRANCHING_FACTOR + previous_level_start_pos;
        auto last_child_index =
            first_child_index +
            std::min<std::size_t>(BRANCHING_FACTOR,
                                  nodes_in_previous_level -
                                      current_node_index * BRANCHING_FACTOR);

        for (auto child_node_index :
             ranges::views::iota(first_child_index, last_child_index)) {
          parent_node.minimum_bounding_rectangle.merge_bounding_boxes(
              m_search_tree[child_node_index].minimum_bounding_rectangle);
        }
        m_search_tree.emplace_back(parent_node);
      }
      nodes_in_previous_level = nodes_in_current_level;
      tree_level_sizes.push_back(nodes_in_previous_level);
    }

    std::reverse(m_search_tree.begin(), m_search_tree.end());
    std::reverse(tree_level_sizes.begin(), tree_level_sizes.end());

    m_tree_level_starts = {0};
    std::partial_sum(tree_level_sizes.begin(), tree_level_sizes.end(),
                     std::back_inserter(m_tree_level_starts));
    assert(m_tree_level_starts.size() >= 2);

    for (auto index : ranges::views::iota(0, tree_level_sizes.size())) {
      std::reverse(m_search_tree.begin() + m_tree_level_starts[index],
                   m_search_tree.begin() + m_tree_level_starts[index] +
                       tree_level_sizes[index]);
    }
  }

  template <typename std::enable_if<O == storage::Ownership::Container>>
  explicit StaticRTree(const std::filesystem::path &on_disk_filename,
                       const Vector<Coordinate> &coordinate_list)
      : m_coordinate_list(coordinate_list.data(), coordinate_list.size()) {
    m_objects = mmap_file<E>(on_disk_filename, m_objects_region);
  }

  /*explicit StaticRTree(Vector<TreeNode> search_tree,
                       Vector<std::uint64_t> tree_level_starts,
                       const std::filesystem::path &on_disk_filename,
                       const Vector<Coordinate> &coordinate_list)
      : m_search_tree(std::move(search_tree)),
        m_coordinate_list(coordinate_list.data(), coordinate_list.size()),
        m_tree_level_starts(std::move(tree_level_starts)) {

    assert(m_tree_level_starts.size() > 1);
    m_objects = mmap_file<E>(on_disk_filename, m_objects_region);
  }*/

  std::vector<E> search_in_box(const RectangleInt2D &search_rectangle) const {
    const RectangleInt2D projected_rectangle{
        search_rectangle.min_lon, search_rectangle.max_lon,
        to_fixed(FloatLatitude{web_mercator::lat_to_y(
            to_floating(FixedLatitude(search_rectangle.min_lat)))}),
        to_fixed(FloatLatitude{web_mercator::lat_to_y(
            to_floating(FixedLatitude(search_rectangle.max_lat)))})};
    std::vector<E> results;
    std::queue<TreeIndex> traversal_queue;
    traversal_queue.push(TreeIndex{});

    while (!traversal_queue.empty()) {
      auto const current_tree_index = traversal_queue.front();
      traversal_queue.pop();

      if (is_leaf(current_tree_index)) {
        for (const auto current_child_index :
             child_indices(current_tree_index)) {
          const auto &current_edge = m_objects[current_child_index];

          const RectangleInt2D bounding_box{
              std::min(m_coordinate_list[current_edge.u].longitude,
                       m_coordinate_list[current_edge.v].longitude),
              std::max(m_coordinate_list[current_edge.u].longitude,
                       m_coordinate_list[current_edge.v].longitude),
              std::min(m_coordinate_list[current_edge.u].latitude,
                       m_coordinate_list[current_edge.v].latitude),
              std::max(m_coordinate_list[current_edge.u].latitude,
                       m_coordinate_list[current_edge.v].latitude),
          };

          if (bounding_box.intersects(search_rectangle)) {
            results.push_back(current_edge);
          }
        }
      } else {
        assert(current_tree_index + 1 < m_tree_level_starts.size());

        for (const auto child_index : child_indices(current_tree_index)) {
          const auto &child_rectangle =
              m_search_tree[child_index].minimum_bounding_rectangle;

          if (child_rectangle.intersects(projected_rectangle)) {
            traversal_queue.push(TreeIndex(
                current_tree_index.level + 1,
                child_index -
                    m_tree_level_starts[current_tree_index.level + 1]));
          }
        }
      }
    }
    return results;
  }

  std::vector<E> nearest(const Coordinate input_coordinate,
                         const std::size_t &max_results) const {
    return nearest(
        input_coordinate,
        [](const CandidateSegment &) { return std::make_pair(true, true); },
        [max_results](const std::size_t num_results, const CandidateSegment &) {
          return num_results >= max_results;
        });
  }

  template <typename F, typename M>
  std::vector<E> nearest(const Coordinate input_coordinate, const F filter,
                         const M terminate) const {
    std::vector<E> results;
    auto projected_coordinate = web_mercator::from_wgs84(input_coordinate);
    Coordinate fixed_projected_coordinate{projected_coordinate};
    std::priority_queue<QueryCandidate> traversal_queue;
    traversal_queue.push(QueryCandidate{0, TreeIndex{}});

    while (!traversal_queue.empty()) {
      QueryCandidate current_query_node = traversal_queue.top();
      traversal_queue.pop();

      const TreeIndex &current_tree_index = current_query_node.tree_index;

      if (!current_query_node.is_segment()) {
        if (is_leaf(current_tree_index))
          explore_leaf_node(current_tree_index, fixed_projected_coordinate,
                            projected_coordinate, traversal_queue);
        else
          explore_tree_node(current_tree_index, fixed_projected_coordinate,
                            traversal_queue);
      } else {
        auto edge_data = m_objects[current_query_node.segment_index];
        const auto &current_candidate = CandidateSegment{
            current_query_node.fixed_projected_coordinate, edge_data};

        if (terminate(results.size(), current_candidate))
          break;

        auto use_segment = filter(current_candidate);

        if (!use_segment.first and !use_segment.second)
          continue;

        edge_data.forward_segment_id.enabled &= use_segment.first;
        edge_data.forward_segment_id.enabled &= use_segment.second;
        results.push_back(std::move(edge_data));
      }
    }
    return results;
  }
};
} // namespace util
} // namespace osram
#endif
