#ifndef OSRAM_ENGINE_APPROACH
#define OSRAM_ENGINE_APPROACH
#include <cstdint>
namespace osram {
namespace engine {
enum class Approach : std::uint8_t {
  CURB = 0,
  UNRESTRICTED = 0,
};
}
} // namespace osram
#endif
