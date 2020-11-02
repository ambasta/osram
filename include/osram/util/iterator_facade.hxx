#ifndef OSRAM_UTIL_ITERATOR_FACADE
#define OSRAM_UTIL_ITERATOR_FACADE

#include <cstddef>
#include <memory>
#include <string>

namespace osram {
namespace util {
template <class Reference> struct arrow_proxy {
  Reference r;
  Reference *operator->() { return &r; }
};

template <typename T> concept impls_decerement = requires(T it) {
  it.decrement();
};

template <typename Derived> class iterator_facade {
public:
  using self_type = Derived;

private:
  self_type &_self() { return static_cast<self_type &>(*this); }
  const self_type &_self() const {
    return static_cast<const self_type &>(*this);
  }

public:
  decltype(auto) operator*() const { return _self().dereference(); }

  auto operator->() const {
    decltype(auto) ref = *this;

    if constexpr (std::is_reference_v<decltype(ref)>) {
      return std::addressof(ref);
    } else {
      return arrow_proxy(std::move(ref));
    }
  }

  friend bool operator==(const self_type &lhs, const self_type &rhs) {
    return lhs.equal_to(rhs);
  }

  self_type &operator++() {
    _self().increment();
    return _self();
  }

  self_type &operator++(int) {
    auto copy = _self();
    ++*this;
    return copy;
  }

  self_type &operator--() requires impls_decerement<self_type> {
    _self().decrement();
    return _self();
  }

  self_type &operator--(int) requires impls_decerement<self_type> {
    auto copy = _self();
    --*this;
    return copy;
  }
};

template <typename T> concept impls_distance_to = requires(const T it) {
  it.distance_to(it);
};

template <typename Iter>
requires std::is_base_of_v<iterator_facade<Iter>, Iter> struct iterator_traits {
  static const Iter &_it;
  using reference = decltype(*_it);
  using pointer = decltype(*_it.operator->());
  using difference_type = decltype(_it.distance_to(_it));
};
} // namespace util
} // namespace osram
#endif
