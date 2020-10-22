#ifndef OSRAM_ENGINE_ALGORITHM
#define OSRAM_ENGINE_ALGORITHM

#include <type_traits>

namespace osram {
namespace engine {
namespace algorithm {
namespace ch {
struct Algorithm final {};
} // namespace ch

namespace mld {
struct Algorithm final {};
} // namespace mld

template <typename T> const char *name();
template <> inline const char *name<ch::Algorithm>() { return "CH"; }
template <> inline const char *name<mld::Algorithm>() { return "MLD"; }

template <typename T> const char *identifier();
template <> inline const char *identifier<ch::Algorithm>() { return "ch"; }
template <> inline const char *identifier<mld::Algorithm>() { return "mld"; }

template <typename T>
struct has_alternative_path_search final : std::false_type {};

template <typename T>
struct has_shortest_path_search final : std::false_type {};

template <typename T>
struct has_direct_shortest_path_search final : std::false_type {};
template <typename T> struct has_map_matching final : std::false_type {};
template <typename T> struct has_many_to_many_search final : std::false_type {};
template <typename T>
struct supports_distance_annotation_type final : std::false_type {};
template <typename T> struct has_get_tile_turns final : std::false_type {};
template <typename T> struct has_exclude_flags final : std::false_type {};

template <>
struct has_alternative_path_search<ch::Algorithm> final : std::true_type {};
template <>
struct has_shortest_path_search<ch::Algorithm> final : std::true_type {};
template <>
struct has_direct_shortest_path_search<ch::Algorithm> final : std::true_type {};
template <> struct has_map_matching<ch::Algorithm> final : std::true_type {};
template <>
struct has_many_to_many_search<ch::Algorithm> final : std::true_type {};
template <>
struct supports_distance_annotation_type<ch::Algorithm> final : std::true_type {
};
template <> struct has_get_tile_turns<ch::Algorithm> final : std::true_type {};
template <> struct has_exclude_flags<ch::Algorithm> final : std::true_type {};

template <>
struct has_alternative_path_search<mld::Algorithm> final : std::true_type {};
template <>
struct has_direct_shortest_path_search<mld::Algorithm> final : std::true_type {
};
template <>
struct has_shortest_path_search<mld::Algorithm> final : std::true_type {};
template <> struct has_map_matching<mld::Algorithm> final : std::true_type {};
template <>
struct has_many_to_many_search<mld::Algorithm> final : std::true_type {};
template <>
struct supports_distance_annotation_type<mld::Algorithm> final
    : std::false_type {};
template <> struct has_get_tile_turns<mld::Algorithm> final : std::true_type {};
template <> struct has_exclude_flags<mld::Algorithm> final : std::true_type {};
} // namespace algorithm
} // namespace engine
} // namespace osram
#endif
