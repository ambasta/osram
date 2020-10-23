#include <memory>
#include <osram/engine/algorithm.hxx>
#include <osram/engine/api/table_parameters.hxx>
#include <osram/engine/engine.hxx>
#include <osram/engine/engine_config.hxx>
#include <osram/osram.hxx>
#include <osram/status.hxx>

namespace osram {
OSRAM::OSRAM(engine::EngineConfig &config) {
  using CH = engine::algorithm::ch::Algorithm;

  if (!config.use_shared_memory && !config.storage_config.is_valid()) {
    throw std::runtime_error("Required files are missing, cannot continue.  "
                             "Have all the pre-processing steps been run?");
  }

  switch (config.algorithm) {
  case engine::EngineConfig::Algorithm::CH:
    m_engine = std::make_unique<engine::Engine<CH>>(config);
    break;

  /* case engine::EngineConfig::Algorithm::MLD:
    m_engine = std::make_unique<engine::Engine<MLD>>(config);
    break; */
  default:
    std::runtime_error("Algorithm not implemented");
  }
}

OSRAM::~OSRAM() = default;
OSRAM::OSRAM(OSRAM &&) noexcept = default;
OSRAM &OSRAM::operator=(OSRAM &&) noexcept = default;

engine::Status
OSRAM::Table(const engine::api::TableParameters &parameters) const {
  return m_engine->Table(parameters);
}
} // namespace osram
