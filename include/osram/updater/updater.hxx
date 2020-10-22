#ifndef OSRAM_UPDATER
#define OSRAM_UPDATER

#include <osram/updater/updater_config.hxx>

namespace osram {
namespace updater {
class Updater {
private:
  UpdaterConfig m_config;

public:
  Updater(UpdaterConfig config) : m_config{std::move(config)} {}
};
} // namespace updater
} // namespace osram
#endif
