#ifndef OSRAM_EXTRACTOR_SERIALIZATION
#define OSRAM_EXTRACTOR_SERIALIZATION

#include <osram/extractor/conditional_turn_penalty.hxx>
#include <osram/extractor/maneuver_override.hxx>
#include <osram/extractor/nbg_to_ebg.hxx>
#include <osram/extractor/profile_properties.hxx>
#include <osram/extractor/segment_data_container.hxx>

// #include <osram/storage/io.hxx>
// #include <osram/storage/serialization.hxx>

namespace osram {
namespace extractor {
namespace serialization {
template <storage::Ownership O>
inline void write(storage::tar::FileWriter &writer, std::string_view name,
                  const ProfileProperties &properties) {
}
} // namespace serialization
} // namespace extractor
} // namespace osram

#endif
