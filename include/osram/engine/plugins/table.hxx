#ifndef OSRAM_ENGINE_PLUGINS_TABLE
#define OSRAM_ENGINE_PLUGINS_TABLE

#include "osram/engine/routing_algorithm.hxx"
#include "osram/engine/status.hxx"
#include <osram/engine/api/table_parameters.hxx>
#include <osram/engine/plugins/plugin_base.hxx>

namespace osram {
namespace engine {
namespace plugins {
class TablePlugin final : public BasePlugin {
private:
  const int max_locations_distance_table;

public:
  explicit TablePlugin(const int);

  Status handle_request(const RoutingAlgorithmsInterface &,
                        const api::TableParameters &) const;
};
} // namespace plugins
} // namespace engine
} // namespace osram
#endif
