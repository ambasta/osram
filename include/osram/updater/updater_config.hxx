#ifndef OSRAM_UPDATER_UPDATER_CONFIG
#define OSRAM_UPDATER_UPDATER_CONFIG

#include <ctime>
#include <filesystem>

#include <osram/storage/io_config.hxx>
#include <osram/storage/storage_config.hxx>
#include <vector>

namespace osram {
namespace updater {
struct UpdaterConfig final : storage::IOConfig {
  double log_edge_updates_factor;
  std::time_t valid_now;

  std::vector<std::string> segment_speed_lookup_paths,
      turn_penalty_lookup_paths;
  std::string tz_file_path;

  UpdaterConfig()
      : IOConfig({".osrm.ebg", ".osrm.turn_weight_penalties",
                  ".osrm.turn_duration_penalties", ".osrm.turn_penalties_index",
                  ".osrm.nbg_nodes", ".osrm.ebg_nodes", ".osrm.geometry",
                  ".osrm.fileIndex", ".osrm.properties", ".osrm.restrictions",
                  ".osrm.enw"},
                 {}, {".osrm.datasource_names"}),
        valid_now(0) {}

  void use_default_output_names(const std::filesystem::path &base) {
    IOConfig::useDefaultOutputNames(base);
  }
};
} // namespace updater
} // namespace osram
#endif
