#ifndef OSRAM_UTIL_REF
#define OSRAM_UTIL_REF

#include <type_traits>

#include <osram/util/attrib.hxx>

namespace osram {
namespace util {

template <typename T> struct make_cref {};

template <typename T> struct make_cref<T &> { using type = const T &; };

template <typename T> struct make_cref<T &&> { using type = const T &&; };

template <typename T> using make_cref_t = typename make_cref<T>::type;

template <typename T> extern T &&ref_v;

template <typename T> extern make_cref_t<T &&> cref_v;

template <typename T> struct wrap_refs { using type = T; };

template <typename T>
requires std::is_reference_v<T> //
    struct wrap_refs<T> {
  using type = std::reference_wrapper<std::remove_reference_t<T>>;
};

template <typename T> using wrap_refs_t = typename wrap_refs<T>::type;

template <typename T>
using wrap_if_reference_t [[deprecated("Use wrap_refs_t")]] = wrap_refs_t<T>;

template <typename T> ALWAYS_INLINE constexpr T &&unref(T &&t) noexcept {
  return static_cast<T &&>(t);
}

template <typename T>
ALWAYS_INLINE constexpr T &unref(std::reference_wrapper<T> t) noexcept {
  return t;
}

#define DECL_UNREF_GETTER(FuncName, MemName)                               \
  constexpr auto &&FuncName() &noexcept {                                      \
    return ::osram::util::unref(this->MemName);                                        \
  }                                                                            \
  constexpr auto &&FuncName() const &noexcept {                                \
    return ::osram::util::unref(this->MemName);                                        \
  }                                                                            \
  constexpr auto &&FuncName() &&noexcept {                                     \
    return ::osram::util::unref(                                                       \
        static_cast<decltype(this->MemName) &&>(this->MemName));               \
  }                                                                            \
  static_assert(true)

struct lref_fn {
  template <typename T> constexpr T &operator()(T &&t) const noexcept {
    return t;
  }

  template <typename Left>
  constexpr friend Left &operator|(Left &&l, lref_fn) noexcept {
    return l;
  }
};

inline constexpr lref_fn lref;

} // namespace util
} // namespace osram
#endif
