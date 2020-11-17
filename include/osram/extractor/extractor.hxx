#ifndef OSRAM_EXTRACTOR_EXTRACTOR
#define OSRAM_EXTRACTOR_EXTRACTOR

#include <osram/extractor/config.hxx>
#include <osram/extractor/edge_based_edge.hxx>
#include <osram/extractor/edge_based_graph.hxx>

namespace osram {
namespace extractor {

class ScriptingEnvironment;

class Extractor {
private:
  ExtractorConfig config;

public:
  Extractor(ExtractorConfig);
  int run(ScriptingEnvironment &);

private:
  void parse_osm_data(ScriptingEnvironment &, const std::size_t);
};
} // namespace extractor
} // namespace osram
#endif
