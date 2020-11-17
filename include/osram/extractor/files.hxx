#ifndef OSRAM_EXTRACTOR_FILES
#define OSRAM_EXTRACTOR_FILES
#include <filesystem>

#include <osram/extractor/node_data_container.hxx>
#include <osram/extractor/profile_properties.hxx>

#include <osram/storage/serialization.hxx>
#include <osram/storage/tar.hxx>

namespace osram {
namespace extractor {
namespace files {
template <typename T>
inline void write_timestamp(const std::filesystem::path &path,
                            const T &timestamp) {
  const auto fingerprint = storage::tar::FileWriter::GenerateFingerPrint;
  storage::tar::FileWriter writer(path, fingerprint);

  storage::serialization::write(writer, "/common/timestamp", timestamp);
}
} // namespace files
} // namespace extractor
} // namespace osram
#endif
