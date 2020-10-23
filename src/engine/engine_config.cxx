#include <osram/engine/engine_config.hxx>

namespace osram {
namespace engine {
bool EngineConfig::is_valid() const {
  std::string_view path{""};
  const bool all_paths_are_emtpy = storage_config.get_path(path).empty();

  const auto unlimited_or_more_than = [](const auto value, const auto limit) {
    return value == -1 or value > limit;
  };

  const bool limits_valid =
      unlimited_or_more_than(max_locations_distance_table, 2) and
      unlimited_or_more_than(max_locations_map_matching, 2) and
      unlimited_or_more_than(max_radius_map_matching, 0) and
      unlimited_or_more_than(max_locations_trip, 2) and
      unlimited_or_more_than(max_locations_viaroute, 2) and
      unlimited_or_more_than(max_results_nearest, 0) and max_alternatives >= 0;

  return ((use_shared_memory and all_paths_are_emtpy) or
          storage_config.is_valid()) and
         limits_valid;
}
} // namespace engine
} // namespace osram
