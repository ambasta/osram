#ifndef OSRAM_GUIDANCE_TURN_BEARING
#define OSRAM_GUIDANCE_TURN_BEARING

#include <cassert>
#include <cstdint>

namespace osram {
namespace guidance {
namespace {
const double bearing_scale = 360.0 / 256.0;
}

class TurnBearing {
private:
  std::uint8_t bearing;

public:
  TurnBearing(const double value = 0) : bearing(value / bearing_scale) {
    assert(value >= 0 and value < 360.0);
  }

  double get() const { return bearing * bearing_scale; }
};
} // namespace guidance
} // namespace osram
#endif
