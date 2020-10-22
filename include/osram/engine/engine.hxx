#ifndef OSRAM_ENGINE
#define OSRAM_ENGINE

#include <osram/engine/algorithm.hxx>
#include <osram/engine/api/table_parameters.hxx>
#include <osram/engine/datafacade_provider.hxx>
#include <osram/engine/engine_config.hxx>
#include <osram/engine/plugins/table.hxx>
#include <osram/engine/routing_algorithm.hxx>

namespace osram {
namespace engine {
class EngineInterface {
public:
  virtual ~EngineInterface() = default;
  virtual Status Table(const api::TableParameters &) const = 0;
};

template <typename T> class Engine final : public EngineInterface {
private:
  std::unique_ptr<DataFacadeProvider<T>> facade_provider;
  mutable SearchEngineData<T> heaps;
  const plugins::TablePlugin table_plugin;

  template <typename P> auto get_algorithms(const P &params) const {
    return RoutingAlgorithms<T>{heaps, facade_provider->get(params)};
  }

public:
  Status Table(const api::TableParameters &params) const override final {
    return table_plugin.handle_request(get_algorithms(params), params);
  }
};
} // namespace engine
} // namespace osram
#endif
