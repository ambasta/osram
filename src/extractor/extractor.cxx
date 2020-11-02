#include <osram/extractor/extractor.hxx>
#include <osram/extractor/turn_lane_types.hxx>
#include <tbb/global_control.h>
#include <thread>

#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/flex_mem.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/thread/pool.hpp>
#include <osmium/visitor.hpp>

namespace osram {
namespace extractor {
Extractor::Extractor(ExtractorConfig config) : config(std::move(config)) {}

int Extractor::run(ScriptingEnvironment &env) {
  const std::size_t system_concurrency = std::thread::hardware_concurrency();
  const std::size_t num_threads =
      std::min(system_concurrency, config.num_threads);
  tbb::global_control gc(tbb::global_control::max_allowed_parallelism,
                         config.num_threads);

  parse_osm_data(env, num_threads);
  return 0;
}

void Extractor::parse_osm_data(ScriptingEnvironment &env,
                               const std::size_t concurrency) {
  const osmium::io::File input_file(config.input_path.string());
  osmium::thread::Pool pool(concurrency);
  {
    const osmium::io::Reader reader(input_file, pool,
                                    osmium::osm_entity_bits::nothing);
    osmium::io::Header header = reader.header();

    std::string generator = header.get("generator");

    if (generator.empty()) {
        generator = "unknown tool";
    }

    std::string timestamp = header.get("osmosis_replication_timestamp");

    if(config.data_version == "osmosis") files::
  }
}
} // namespace extractor
} // namespace osram
