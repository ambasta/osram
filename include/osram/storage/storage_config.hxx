#ifndef OSRAM_STORAGE_STORAGE_CONFIG
#define OSRAM_STORAGE_STORAGE_CONFIG
#include <osram/storage/io_config.hxx>

namespace osram {
namespace storage {
struct StorageConfig final : IOConfig {
  StorageConfig()
      : IOConfig({".osrm.ramIndex", ".osrm.fileIndex", ".osrm.edges",
                  ".osrm.geometry", ".osrm.turn_weight_penalties",
                  ".osrm.turn_duration_penalties", ".osrm.datasource_names",
                  ".osrm.names", ".osrm.timestamp", ".osrm.properties",
                  ".osrm.icd", ".osrm.maneuver_overrides"},
                 {".osrm.hsgr", ".osrm.nbg_nodes", ".osrm.ebg_nodes",
                  ".osrm.cells", ".osrm.cell_metrics", ".osrm.mldgr",
                  ".osrm.tld", ".osrm.tls", ".osrm.partition"},
                 {}) {}
  StorageConfig(const std::filesystem::path &base) : StorageConfig() {
    IOConfig::use_default_output_names(base);
  }
};
} // namespace storage
} // namespace osram
#endif
