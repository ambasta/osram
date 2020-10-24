#include <iterator>
#include <osram/engine/base64.hxx>
#include <osram/engine/datafacade/datafacade_base.hxx>
#include <osram/engine/hint.hxx>

namespace osram {
namespace engine {
bool Hint::is_valid(const util::Coordinate new_coordinates,
                    const datafacade::BaseDataFacade &facade) const {
  auto is_same_coordinate =
      new_coordinates.longitude == phantom_node.input_location.longitude and
      new_coordinates.latitude == phantom_node.input_location.latitude;
  return is_same_coordinate and phantom_node.is_valid() and
         facade.get_checksum() == data_checksum;
}

std::string Hint::to_base64() const {
  auto base64 = encode_base64_bytewise(*this);
  std::replace(std::begin(base64), std::end(base64), '+', '-');
  std::replace(std::begin(base64), std::end(base64), '/', '_');
  return base64;
}

Hint Hint::from_base64(std::string_view base64hint) {
  assert(("Hint has invalid size", base64hint.size() == ENCODED_HINT_SIZE));
  std::string encoded = base64hint.data();
  std::replace(encoded.begin(), encoded.end(), '-', '+');
  std::replace(encoded.begin(), encoded.end(), '_', '/');

  return decode_base64_bytewise<Hint>(encoded);
}

} // namespace engine
} // namespace osram
