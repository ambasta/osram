#ifndef OSRAM_IO_CONFIG
#define OSRAM_IO_CONFIG
#include <array>
#include <filesystem>
#include <vector>

namespace osram {
namespace storage {
struct IOConfig {
private:
  std::vector<std::filesystem::path> required_input_files, optional_input_files,
      output_files;

  static bool is_configured(std::string_view filename,
                            const std::vector<std::filesystem::path> &paths) {

    for (auto &path : paths)
      if (path.string().ends_with(filename))
        return true;
    return false;
  }

protected:
  void useDefaultOutputNames(const std::filesystem::path &base) {
    std::array<std::string, 6> known_extensions{
        {".osm.bz2", ".osm.pbf", ".osm.xml", ".pbf", ".osm", ".osrm"}};
    base_path = base;
    for (auto &ext : known_extensions) {
      if (base_path.extension() == ext)
        base_path.replace_extension("");
    }
  }

public:
  std::filesystem::path base_path;

  IOConfig(std::vector<std::filesystem::path> required_input_files,
           std::vector<std::filesystem::path> optional_input_files,
           std::vector<std::filesystem::path> output_files)
      : required_input_files(required_input_files),
        optional_input_files(optional_input_files), output_files(output_files) {
  }

  bool isValid() const;

  std::filesystem::path getPath(std::string_view &filename) const {
    if (!is_configured(filename, required_input_files) &&
        !is_configured(filename, optional_input_files) &&
        !is_configured(filename, output_files)) {
      throw std::invalid_argument(
          "Tried to access file which isn't configured: " +
          std::string{filename});
    }
    return std::filesystem::path{base_path.c_str() + std::string{filename}};
  }
};
} // namespace storage
} // namespace osram
#endif
