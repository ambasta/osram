#ifndef OSRAM_EXTRACTOR_FILES
#define OSRAM_EXTRACTOR_FILES
#include <filesystem>
namespace osram {
namespace extractor {
namespace files {
template <typename T>
inline void write_timestamp(const std::filesystem::path &path,
                            const T &timestamp) {
  const auto fingerprint = storage::tar::FileWriter::GenerateFingerPrint;
  storage::tar::FileWriter writer(path, fingerprint);

  storage::serialization::writer(writer, "/common/timestamp", timestamp);
}
} // namespace files
} // namespace extractor
} // namespace osram
#endif
