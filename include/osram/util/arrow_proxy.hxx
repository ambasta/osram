#ifndef OSRAM_UTIL_ARROW_PROXY
#define OSRAM_UTIL_ARROW_PROXY
#include <osram/util/fwd.hxx>
#include <osram/util/ref.hxx>
namespace osram {
namespace util {
template <typename T> class arrow_proxy {
  wrap_refs_t<T> _value;

public:
  explicit constexpr arrow_proxy(T &&t) noexcept : _value(OS_FWD(t)) {}

  constexpr auto &operator*() noexcept { return unref(_value); }
  constexpr auto &operator*() const noexcept { return unref(_value); }

  constexpr auto operator->() noexcept { return std::addressof(**this); }
  constexpr auto operator->() const noexcept { return std::addressof(**this); }
};

template <typename T> arrow_proxy(T &&) -> arrow_proxy<T>;
} // namespace util
} // namespace osram
#endif
