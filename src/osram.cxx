#include <osram/engine/algorithm.hxx>
#include <osram/osram.hxx>

namespace osram {
OSRAM::OSRAM(engine::EngineConfig &config) {
  using CH = engine::algorithm::ch::Algorithm;

  if (!config.use_shared_memory && !config.storage_config.is_valid()) {
    throw std::runtime_error("Required files are missing, cannot continue.  "
                             "Have all the pre-processing steps been run?");
  }
}
} // namespace osram
