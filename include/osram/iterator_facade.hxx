#ifndef OSRAM_UTIL_ITERATOR_FACADE
#define OSRAM_UTIL_ITERATOR_FACADE
#include <iterator>

#include <osram/util/arrow_proxy.hxx>

namespace osram {
namespace util {
namespace detail {
template <typename T>
concept iter_has_distance_to_method = requires(const T t) {
  t.distance_to(t);
};

template <typename T> struct infer_difference_type {
  using type = std::ptrdiff_t;
};

template <iter_has_distance_to_method T> struct infer_difference_type<T> {
  static const T &_it;
  using type = decltype(_it.distance_to(_it));
};

template <typename T>
using infer_difference_type_t = typename infer_difference_type<T>::type;

template <typename T> struct infer_value_type {
  static const T &_it;
  using type = std::remove_const_t<std::remove_reference_t<decltype(*_it)>>;
};

template <typename T> requires requires { typename T::value_type; }
struct infer_value_type<T> {
  using type = typename T::value_type;
};

template <typename T>
using infer_value_type_t = typename infer_value_type<T>::type;

template <typename T> concept iter_has_equal_to_method = requires(const T it) {
  it.equal_to(it);
};

template <typename T> concept iter_has_increment_method = requires(T t) {
  t.increment();
};

template <typename T> concept iter_has_decrement_method = requires(T t) {
  t.decrement();
};

template <typename T>
concept iter_has_advance_method = requires(T t,
                                           const infer_difference_type_t<T> d) {
  t.advance(d);
};

template <typename T>
concept iter_is_random_access =
    iter_has_distance_to_method<T> &&iter_has_advance_method<T>;

template <typename T>
concept iter_is_bidirectional =
    iter_is_random_access<T> || iter_has_decrement_method<T>;

template <typename T>
concept iter_is_single_pass = bool(T::single_pass_iterator);

template <typename T, typename Iter>
concept iter_sentinel = std::is_same_v<T, typename Iter::sentinel_type>;

template <typename Iter>
concept equal_compare_check =
    iter_has_equal_to_method<Iter> || iter_has_distance_to_method<Iter>;

template <typename T, typename Iter>
concept iter_diff = std::is_convertible_v<T, infer_difference_type_t<Iter>>;

struct iterator_facade_base {};
} // namespace detail

template <typename Derived>
class iterator_facade : detail::iterator_facade_base {
public:
  using self_type = Derived;

private:
  constexpr self_type &_self() noexcept {
    return static_cast<self_type &>(*this);
  }
  constexpr const self_type &_self() const noexcept {
    return static_cast<const self_type &>(*this);
  }

public:
  [[nodiscard]] constexpr decltype(auto) operator*() const noexcept {
    return _self().dereference();
  }

  constexpr decltype(auto) operator->() const noexcept {
    decltype(auto) deref = **this;
    if constexpr (std::is_reference_v<decltype(deref)>) {
      return std::addressof(deref);
    } else {
      return arrow_proxy{std::move(deref)};
    }
  }

  [[nodiscard]] constexpr friend auto operator-(const self_type &left,
                                                const self_type &right) noexcept
      requires detail::iter_is_random_access<self_type> {
    return right.distance_to(left);
  }

  constexpr self_type &operator++() noexcept {
    if constexpr (detail::iter_has_increment_method<self_type>) {
      // If there is an increment(), assume it is the most efficient way to
      // advance, even if we have an advance()
      _self().increment();
    } else if constexpr (detail::iter_is_random_access<self_type>) {
      // Just offset by one
      _self() += 1;
    } else {
      // Bad!
      static_assert(detail::iter_is_random_access<self_type>,
                    "Iterator subclass must provide an `increment` or "
                    "`advance(n)` method");
    }
    return _self();
  }

  constexpr auto operator++(int) noexcept {
    if constexpr (detail::iter_is_single_pass<self_type>) {
      // The iterator is a single-pass iterator. It isn't safe to make and
      // return an old copy.
      ++*this;
    } else {
      auto cp = _self();
      ++*this;
      return cp;
    }
  }

  constexpr self_type &operator--() noexcept
      requires detail::iter_is_bidirectional<self_type> {
    if constexpr (detail::iter_has_decrement_method<self_type>) {
      _self().decrement();
    } else {
      _self() -= 1;
    }
    return _self();
  }

  constexpr self_type operator--(int) noexcept
      requires detail::iter_is_bidirectional<self_type> {
    auto cp = _self();
    --*this;
    return cp;
  }

  template <detail::iter_diff<self_type> Diff>
  [[nodiscard]] constexpr friend self_type operator+(self_type left,
                                                     Diff off) noexcept
      requires detail::iter_is_random_access<self_type> {
    return left += off;
  }

  template <detail::iter_diff<self_type> D>
  [[nodiscard]] constexpr friend self_type
  operator+(D off, const self_type &self) noexcept
      requires detail::iter_is_random_access<self_type> {
    return self + off;
  }

  template <detail::iter_diff<self_type> D>
  [[nodiscard]] constexpr friend self_type operator-(const self_type &self,
                                                     D off) noexcept
      requires detail::iter_is_random_access<self_type> {
    using diff_type = detail::infer_difference_type_t<self_type>;
    using signed_diff_type = std::make_signed_t<diff_type>;
    return self + -static_cast<signed_diff_type>(off);
  }

  template <detail::iter_sentinel<self_type> S>
  [[nodiscard]] constexpr friend auto
  operator-(S s, const self_type &self) noexcept {
    return self.distance_to(s);
  }

  template <detail::iter_diff<self_type> D>
  constexpr friend self_type &operator+=(self_type &self, D off) noexcept
      requires detail::iter_is_random_access<self_type> {
    self.advance(off);
    return self;
  }

  template <detail::iter_diff<self_type> D>
  constexpr friend self_type &operator-=(self_type &self, D off) noexcept
      requires detail::iter_is_random_access<self_type> {
    return self = self - off;
  }

  template <detail::iter_diff<self_type> D>
  [[nodiscard]] constexpr decltype(auto) operator[](D pos) const noexcept
      requires detail::iter_is_random_access<self_type> {
    return *(_self() + pos);
  }

  constexpr bool operator==(const self_type &other) const noexcept
      requires detail::equal_compare_check<self_type> {
    if constexpr (detail::iter_has_equal_to_method<self_type>) {
      return _self().equal_to(other);
    } else if constexpr (detail::iter_has_distance_to_method<self_type>) {
      return _self().distance_to(other) == 0;
    } else {
      static_assert(detail::iter_has_distance_to_method<self_type>,
                    "Iterator must provide either `distance_to(other)` or "
                    "`equal_to(other)`");
    }
  }

  template <detail::iter_sentinel<self_type> S>
  constexpr bool operator==(S) const noexcept {
    return _self().at_end();
  }

  [[nodiscard]] constexpr auto
  operator<=>(const self_type &right) const noexcept
      requires detail::iter_is_random_access<self_type> {
    auto dist = _self() - right;
    auto rel = dist <=> 0;
    return rel;
  }
};

template <typename Derived, typename InnerIter>
class iterator_wrapper_facade : public iterator_facade<Derived> {
  using base = typename iterator_wrapper_facade::iterator_facade;
  using self_type = Derived;

protected:
  InnerIter wrapped_iter;

public:
  iterator_wrapper_facade() = default;

  explicit iterator_wrapper_facade(InnerIter it) : wrapped_iter(it) {}

  template <detail::iter_diff<self_type> D>
  constexpr void advance(D off) noexcept {
    std::advance(wrapped_iter, off);
  }

  constexpr auto distance_to(const self_type &other) const noexcept
      requires std::is_base_of_v<
          typename std::iterator_traits<InnerIter>::iterator_category,
          std::random_access_iterator_tag> {
    return other.wrapped_iter - wrapped_iter;
  }

  constexpr bool equal_to(const self_type &other) const noexcept {
    return wrapped_iter == other.wrapped_iter;
  }
};
} // namespace util
} // namespace osram

namespace std {
template <typename Derived>
requires std::is_base_of_v<osram::util::detail::iterator_facade_base,
                           Derived> struct iterator_traits<Derived> {
  static const Derived &_const_it;
  using reference = decltype(*_const_it);
  using pointer = decltype(_const_it.operator->());
  using value_type = osram::util::detail::infer_value_type_t<Derived>;
  using difference_type = osram::util::detail::infer_difference_type_t<Derived>;

  using iterator_category = std::conditional_t<
      osram::util::detail::iter_is_random_access<Derived>,
      std::random_access_iterator_tag,
      std::conditional_t<
          osram::util::detail::iter_is_bidirectional<Derived>,
          std::input_iterator_tag,
          std::conditional_t<osram::util::detail::iter_is_single_pass<Derived>,
                             std::input_iterator_tag,
                             std::forward_iterator_tag>>>;
  using iterator_concept = iterator_category;
};
} // namespace std
#endif
