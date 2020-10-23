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
} // namespace engine
} // namespace osram
