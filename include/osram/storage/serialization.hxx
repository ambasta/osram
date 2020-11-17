#ifndef OSRAM_STORAGE_SERIALIZATION
#define OSRAM_STORAGE_SERIALIZATION

#include <osram/storage/io.hxx>
#include <osram/storage/tar.hxx>

#include <osram/util/vector_view.hxx>

#include <string_view>

namespace osram {
namespace storage {
namespace serialization {
inline void write(tar::FileWriter &writer, std::string_view name,
                  std::string_view data) {
  const auto count = data.size();
  writer.write_element_count64(name, count);
  writer.write_from(name, data.data(), count);
}
} // namespace serialization
} // namespace storage
} // namespace osram
#endif
