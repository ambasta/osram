#ifndef OSRAM_HXX
#define OSRAM_HXX

#include <memory>

#include <osram/engine/status.hxx>
#include <osram/osram_fwd.hxx>

namespace osram {
class OSRAM final {
private:
  std::unique_ptr<engine::EngineInterface> m_engine;

public:
  explicit OSRAM(engine::EngineConfig &config);

  ~OSRAM();

  OSRAM(OSRAM &&) noexcept;

  OSRAM &operator=(OSRAM &&) noexcept;

  engine::Status Table(const engine::api::TableParameters &) const;
};
}; // namespace osram
#endif
