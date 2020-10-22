#ifndef OSRAM_UTIL_ALIAS
#define OSRAM_UTIL_ALIAS

#include <compare>
#include <ostream>
#include <type_traits>

namespace osram {
template <typename FROM, typename TAG> struct Alias;
template <typename FROM, typename TAG>
inline std::ostream &operator<<(std::ostream &, const Alias<FROM, TAG> &);

template <typename F, typename T> struct Alias final {
  using value_type = F;
  static_assert(std::is_arithmetic<F>::value,
                "Needs to be based on arithmetic type");

  F value;

  friend std::ostream &operator<<<F, T>(std::ostream &, const Alias &);

  explicit operator F &() { return value; }

  explicit operator F() const { return value; }

  inline Alias operator+(const Alias rhs) const {
    return Alias{value + static_cast<const F>(rhs)};
  }

  inline Alias operator*(const Alias rhs) const {
    return Alias{value * static_cast<const F>(rhs)};
  }

  inline Alias operator*(const double rhs) const { return Alias{value * rhs}; }

  inline Alias operator|(const Alias rhs) const {
    return Alias{value | static_cast<const F>(rhs)};
  }

  inline Alias operator&(const Alias rhs) const {
    return Alias{value & static_cast<const F>(rhs)};
  }

  auto operator<=>(const Alias &) const = default;

  inline Alias operator++() {
    value++;
    return *this;
  }
};

template <typename F, typename T>
inline std::ostream &operator<<(std::ostream &stream,
                                const Alias<F, T> &alias) {
  return stream << alias.value;
}
} // namespace osram

namespace std {
template <typename F, typename T> struct hash<osram::Alias<F, T>> {
  typedef osram::Alias<F, T> argument_type;
  typedef std::size_t result_type;

  std::size_t operator()(osram::Alias<F, T> const &alias) const {
    return std::hash<F>()(static_cast<const F>(alias));
  }
};
} // namespace std
#endif
