#ifndef OSRAM_ENGINE_CONFIG
#define OSRAM_ENGINE_CONFIG
#include <osram/storage/storage_config.hxx>

namespace osram {
namespace engine {
struct EngineConfig final {
  bool is_valid() const;

  enum class Algorithm { CH, MLD };

  storage::StorageConfig storage_config;
  int max_locations_trip = -1;
  int max_locations_viaroute = -1;
  int max_locations_distance_table = -1;
  int max_locations_map_matching = -1;
  double max_radius_map_matching = -1.0;
  int max_results_nearest = -1;
  int max_alternatives = 3;
  bool use_shared_memory = true;
  std::filesystem::path memory_file;
  bool use_mmap = true;
  Algorithm algorithm = Algorithm::CH;
  std::string verbosity;
  std::string dataset_name;
};
} // namespace engine
} // namespace osram
#endif
