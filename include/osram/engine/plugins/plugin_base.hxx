#ifndef OSRAM_ENGINE_PLUGINS_PLUGIN_BASE
#define OSRAM_ENGINE_PLUGINS_PLUGIN_BASE

#include <algorithm>
#include <iterator>
#include <osram/engine/api/base_parameters.hxx>
#include <osram/engine/datafacade/datafacade_base.hxx>
#include <osram/engine/hint.hxx>
#include <osram/engine/phantom_node.hxx>
#include <osram/engine/routing_algorithm.hxx>
#include <osram/engine/status.hxx>
#include <osram/util/coordinate.hxx>
#include <osram/util/coordinate_calculation.hxx>

#include <fmt/format.h>

namespace osram {
namespace engine {
namespace plugins {
class BasePlugin {
protected:
  bool check_all_coordinates(
      const std::vector<util::Coordinate> &coordinates) const {
    return !std::any_of(coordinates.begin(), coordinates.end(),
                        [](const util::Coordinate &coordinate) {
                          return !coordinate.is_valid();
                        });
  }

  bool check_algorithms(const api::BaseParameters &params,
                        const RoutingAlgorithmsInterface &algorithms,
                        std::string &error) {
    if (algorithms.is_valid())
      return true;

    if (!algorithms.has_exclude_flags() and !params.exclude.empty()) {
      error = "NotImplemented: This algorithm does not support exclude flags.";
      return false;
    }

    if (algorithms.has_exclude_flags() and !params.exclude.empty()) {
      error = "InvalidValue: Exclude flag combination is not supported";
      return false;
    }
    // static_assert(false, "There are only two reasons why the algorithm
    // interface can be invalid.");
    return false;
  }

  struct ErrorRenderer {
    std::string code;
    std::string message;

    ErrorRenderer(std::string code, std::string message)
        : code(std::move(code)), message(std::move(message)) {}

    void operator()(std::string &result) {
      result = fmt::format("code={} message={}", code, message);
    }
  };

  std::vector<PhantomNode> snap_phantom_nodes(
      const std::vector<PhantomNodePair> &phantom_node_pair_list) const {
    const auto check_component_id_is_tiny =
        [](const PhantomNodePair &phantom_pair) {
          return phantom_pair.first.component.is_tiny;
        };

    const auto check_all_in_same_component =
        [](const std::vector<PhantomNodePair> &nodes) {
          const auto component_id = nodes.front().first.component.id;

          return std::all_of(
              nodes.begin(), nodes.end(),
              [component_id](const PhantomNodePair &phantom_pair) {
                return component_id == phantom_pair.first.component.id;
              });
        };

    const auto fallback_to_big_component =
        [](const PhantomNodePair &phantom_pair) {
          if (phantom_pair.first.component.is_tiny and
              phantom_pair.second.is_valid() and
              !phantom_pair.second.component.is_tiny)
            return phantom_pair.second;
          return phantom_pair.first;
        };

    const auto use_closed_phantoms = [](const PhantomNodePair &phantom_pair) {
      return phantom_pair.first;
    };

    const bool every_phantom_is_in_tiny_cc =
        std::all_of(phantom_node_pair_list.begin(),
                    phantom_node_pair_list.end(), check_component_id_is_tiny);

    auto all_in_same_component =
        check_all_in_same_component(phantom_node_pair_list);

    std::vector<PhantomNode> snapped_phantoms;
    snapped_phantoms.reserve(phantom_node_pair_list.size());

    if (every_phantom_is_in_tiny_cc and all_in_same_component)
      std::transform(
          phantom_node_pair_list.begin(), phantom_node_pair_list.end(),
          std::back_inserter(snapped_phantoms), fallback_to_big_component);

    return snapped_phantoms;
  }

  std::vector<std::vector<PhantomNodeWithDistance>>
  get_phantom_nodes_in_range(const datafacade::BaseDataFacade &facade,
                             const api::BaseParameters &params,
                             const std::vector<double> radiuses,
                             bool use_all_edges = false) const {
    std::vector<std::vector<PhantomNodeWithDistance>> phantom_nodes(
        params.coordinates.size());
    assert(radiuses.size() == params.coordinates.size());

    const bool use_hints = !params.hints.empty();
    const bool use_bearings = !params.bearings.empty();
    const bool use_approaches = !params.approaches.empty();

    for (std::size_t index = 0; index < params.coordinates.size(); ++index) {
      Approach approach = engine::Approach::UNRESTRICTED;

      if (use_approaches and params.approaches[index])
        approach = params.approaches[index].value();

      if (use_hints and params.hints[index] and
          params.hints[index]->is_valid(params.coordinates[index], facade)) {
        phantom_nodes[index].push_back(PhantomNodeWithDistance{
            params.hints[index]->phantom_node,
            util::coordinate_calculation::haversine_distance(
                params.coordinates[index],
                params.hints[index]->phantom_node.location)});
        continue;
      }

      if (use_bearings and params.bearings[index])
        phantom_nodes[index] = facade.nearest_phantom_nodes_in_range(
            params.coordinates[index], radiuses[index],
            params.bearings[index]->bearing, params.bearings[index]->range,
            approach, use_all_edges);
      else
        phantom_nodes[index] = facade.nearest_phantom_nodes_in_range(
            params.coordinates[index], radiuses[index], approach,
            use_all_edges);
    }
    return phantom_nodes;
  }
};
} // namespace plugins
} // namespace engine
} // namespace osram
#endif
