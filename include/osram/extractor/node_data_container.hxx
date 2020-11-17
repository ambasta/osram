#ifndef OSRAM_EXTRACTOR_NODE_DATA_CONTAINER
#define OSRAM_EXTRACTOR_NODE_DATA_CONTAINER

#include <osram/extractor/class_data.hxx>
#include <osram/extractor/edge_based_node.hxx>
#include <osram/extractor/node_based_edge.hxx>
#include <osram/extractor/travel_mode.hxx>

#include <osram/storage/shared_memory_ownership.hxx>
#include <osram/storage/tar_fwd.hxx>

#include <osram/util/permutation.hxx>
#include <osram/util/typedefs.hxx>
#include <osram/util/vector_view.hxx>

#include <string_view>
#include <vector>

namespace osram {
namespace extractor {
class Extractor;
class EdgeBasedGraphFactory;

namespace detail {
template <storage::Ownership O> class EdgeBasedNodeDataContainerImpl;
}

namespace serialization {
template <storage::Ownership O>
void read(storage::tar::FileReader &reader, std::string_view name,
          detail::EdgeBasedNodeDataContainerImpl<O> &ebn_data);
template <storage::Ownership O>
void write(storage::tar::FileWriter &writer, std::string_view name,
           detail::EdgeBasedNodeDataContainerImpl<O> &ebn_data);
} // namespace serialization

namespace detail {
template <storage::Ownership O> class EdgeBasedNodeDataContainerImpl {
  template <typename T> using Vector = util::ViewOrVector<T, O>;

  friend class osram::extractor::Extractor;
  friend class osram::extractor::EdgeBasedGraphFactory;

private:
  Vector<EdgeBasedNode> nodes;
  Vector<NodeBasedEdgeAnnotation> annotation_data;

public:
  EdgeBasedNodeDataContainerImpl() = default;

  EdgeBasedNodeDataContainerImpl(const NodeId nodes,
                                 const AnnotationId annotation_data)
      : nodes(nodes), annotation_data(annotation_data) {}

  GeometryId get_geometry_id(const NodeId node_id) const {
    return nodes[node_id].geometry_id;
  }

  ComponentId get_component_id(const NodeId node_id) const {
    return nodes[node_id].component_id;
  }

  TravelMode get_travel_mode(const NodeId node_id) const {
    return annotation_data[nodes[node_id].annotation_id].travel_mode;
  }

  bool is_left_hand_driving(const NodeId node_id) const {
    return annotation_data[nodes[node_id].annotation_id].is_left_hand_driving;
  }

  bool is_segregated(const NodeId node_id) const {
    return nodes[node_id].segregated;
  }

  NameId get_name_id(const NodeId node_id) const {
    return annotation_data[nodes[node_id].annotation_id].name_id;
  }

  ClassData get_class_data(const NodeId node_id) const {
    return annotation_data[nodes[node_id].annotation_id].classes;
  }

  /*friend void serialization::write<O>(storage::tar::FileWriter &,
                                      std::string_view,
                                      const EdgeBasedNodeDataContainerImpl &);*/

  template <typename = std::enable_if<O == storage::Ownership::Container>>
  void renumber(const std::vector<std::uint32_t> &permutation) {
    util::inplace_permutation(nodes.begin(), nodes.end(), permutation);
  }

  NodeId number_of_nodes() const { return nodes.size(); }

  AnnotationId number_of_annotations() const { return annotation_data.size(); }

  EdgeBasedNode const &get_node(const NodeId node_id) const {
    return nodes[node_id];
  }

  NodeBasedEdgeAnnotation const &
  get_annotation(const AnnotationId annotation_id) const {
    return annotation_data[annotation_id];
  }
};
} // namespace detail
using EdgeBasedNodeDataExternalContainer =
    detail::EdgeBasedNodeDataContainerImpl<storage::Ownership::External>;
using EdgeBasedNodeDataContainer =
    detail::EdgeBasedNodeDataContainerImpl<storage::Ownership::Container>;
using EdgeBasedNodeDataView =
    detail::EdgeBasedNodeDataContainerImpl<storage::Ownership::View>;
} // namespace extractor
} // namespace osram
#endif
