#ifndef OSRAM_ENGINE_HINT
#define OSRAM_ENGINE_HINT

#include <osram/engine/phantom_node.hxx>
#include <string_view>

namespace osram {
namespace engine {
namespace datafacade {
class BaseDataFacade;
}

struct Hint {
  PhantomNode phantom_node;
  std::uint32_t data_checksum;

  bool is_valid(const util::Coordinate,
                const datafacade::BaseDataFacade &) const;

  std::string to_base64() const;

  static Hint from_base64(std::string_view);

  friend bool operator==(const Hint &, const Hint &);
};

static_assert(sizeof(Hint) == 84 + 4, "Hint is bigger than expected");
constexpr std::size_t ENCODED_HINT_SIZE = 112;
// TODO We've changed size of phantom_node, so this exceeds upstream size
static_assert(ENCODED_HINT_SIZE / 4 * 3 <= sizeof(Hint),
              "ENCODED_HINT_SIZE does not match size of Hint");
} // namespace engine
} // namespace osram
#endif
