#ifndef OSRAM_ENGINE_ROUTING_ALGORITHM
#define OSRAM_ENGINE_ROUTING_ALGORITHM

#include <osram/engine/algorithm.hxx>
#include <osram/engine/datafacade.hxx>
#include <osram/engine/internal_route_result.hxx>
#include <osram/engine/phantom_node.hxx>
#include <osram/engine/search_engine_data.hxx>

namespace osram {
namespace engine {
class RoutingAlgorithmsInterface {
public:
  virtual InternalManyRoutesResult
  alternative_path_search(const PhantomNodes &phantom_node_pair,
                          unsigned number_of_alternatives) const = 0;

  virtual InternalRouteResult shortest_path_search(
      std::vector<PhantomNodes> &phantom_node_pair,
      const std::optional<bool> continue_straight_at_waypoint) const = 0;

  virtual InternalRouteResult
  direct_shortest_path_search(const PhantomNodes &phantom_node_pair) const = 0;

  virtual bool has_exclude_flags() const = 0;
  virtual bool is_valid() const = 0;
};

template <typename T>
class RoutingAlgorithms final : public RoutingAlgorithmsInterface {
private:
  SearchEngineData<T> &m_heaps;
  std::shared_ptr<DataFacade<T>> m_facade;

public:
  RoutingAlgorithms(SearchEngineData<T> &heaps,
                    std::shared_ptr<const DataFacade<T>> facade)
      : m_heaps(heaps), m_facade(facade) {}
};
} // namespace engine
} // namespace osram
#endif
