#ifndef OSRAM_UTIL_VECTOR_VIEW
#define OSRAM_UTIL_VECTOR_VIEW

#include <ranges>
#include <span>
#include <type_traits>
#include <vector>

#include <osram/storage/shared_memory_ownership.hxx>

namespace osram {
namespace util {

template <typename T> class vector_view {
  auto operator()(T *ptr, std::size_t sz) {
    return std::ranges::ref_view{std::span{ptr, sz}};
  }
};

template <typename T, storage::Ownership O>
using InternalOrExternalVector =
    typename std::conditional<O == storage::Ownership::External, std::vector<T>,
                              std::vector<T>>::type;
template <typename T, storage::Ownership O>
using ViewOrVector =
    typename std::conditional<O == storage::Ownership::View, vector_view<T>,
                              InternalOrExternalVector<T, O>>;

template <typename ValueT, typename ContainerT>
struct is_view_or_vector
    : std::integral_constant<
          bool, std::is_same<std::vector<ValueT>, ContainerT>::value or
                    std::is_same<vector_view<ValueT>, ContainerT>::value> {};
} // namespace util
} // namespace osram
#endif
