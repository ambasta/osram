#ifndef OSRAM_STORAGE_SHARED_DATATYPE
#define OSRAM_STORAGE_SHARED_DATATYPE

// #include <osram/storage/block.hxx>
// #include <osram/storage/io_fwd.hxx>

#include <string>
#include <string_view>

namespace osram {
namespace storage {
class BaseDataLayout;

namespace serialization {}

namespace detail {
inline std::string_view trim_name(std::string_view prefix, std::string_view name) {

  if (!prefix.empty() and prefix.back() == '/') {
    auto directory_position = name.find_first_of("/", prefix.length());

    if (directory_position == std::string::npos)
      return name;

    return name.substr(0, directory_position);
  }
  return name;
}
} // namespace detail

class BaseDataLayout {
    protected:
        const Block& get_block(std::string_view name) const {}
};
} // namespace storage
} // namespace osram
#endif
