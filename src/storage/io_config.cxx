#include <algorithm>
#include <filesystem>
#include <osram/storage/io_config.hxx>

namespace osram {
namespace storage {
bool IOConfig::is_valid() const {
  return std::all_of(
      required_input_files.begin(), required_input_files.end(),
      [&](auto &filename) {
        return !std::filesystem::is_regular_file(
            std::filesystem::status(base_path.string() + filename.string()));
      });
}
} // namespace storage
} // namespace osram
