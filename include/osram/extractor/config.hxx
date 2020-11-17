#ifndef OSRAM_EXTRACTOR_EXTRACTOR_CONFIG
#define OSRAM_EXTRACTOR_EXTRACTOR_CONFIG

#include <filesystem>

#include <osram/storage/io_config.hxx>

namespace osram {
namespace extractor {
struct ExtractorConfig final : storage::IOConfig {
  std::filesystem::path input_path, profile_path;
  std::vector<std::filesystem::path> location_dependent_data_paths;
  std::string data_version;

  std::size_t num_threads, small_component_size;

  bool generate_edge_lookup, use_metadata, parse_conditionals,
      use_locations_cache;

  ExtractorConfig() noexcept
      : IOConfig(
            {
                "",
            },
            {},
            {".osrm",
             ".osrm.restrictions",
             ".osrm.names",
             ".osrm.tls",
             ".osrm.tld",
             ".osrm.geometry",
             ".osrm.nbg_nodes",
             ".osrm.ebg_nodes",
             ".osrm.timestamp",
             ".osrm.edges",
             ".osrm.ebg",
             ".osrm.ramIndex",
             ".osrm.fileIndex",
             ".osrm.turn_duration_penalties",
             ".osrm.turn_weight_penalties",
             ".osrm.turn_penalties_index",
             ".osrm.enw",
             ".osrm.properties",
             ".osrm.icd",
             ".osrm.cnbg",
             ".osrm.cnbg_to_ebg",
             ".osrm.maneuver_overrides"}),
        num_threads(0), parse_conditionals(false), use_locations_cache(true) {}

  void use_default_output_names(const std::filesystem::path &base) {
    IOConfig::use_default_output_names(base);
  }
};
} // namespace extractor
} // namespace osram
#endif
