#ifndef OSRAM_UTIL_BIT_RANGE
#define OSRAM_UTIL_BIT_RANGE

#include <bitset>
#include <cassert>
#include <climits>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace osram {
namespace util {
namespace detail {

template <typename T> std::size_t count_ones(T value) {
  static_assert(std::is_unsigned<T>::value, "Only unsigned types allowed");
  std::bitset<sizeof(T) * CHAR_BIT> b_value{value};
  return b_value.count();
}

template <typename T, bool FLAG = true> class index_iterable {
  std::size_t SIZE = sizeof(T) * CHAR_BIT;

public:
  class const_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;

    explicit const_iterator(const std::bitset<sizeof(T) * CHAR_BIT> &bitset)
        : index{static_cast<std::size_t>(-1)}, bitset{bitset} {}

    const_iterator operator++() {
      this->seek_next();
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator prev_this = *this;
      this->seek_next();
      return prev_this;
    }

    const std::size_t &operator*() const { return this->index; }

    bool operator==(const const_iterator &rhs) const {
      return (this->index == rhs.index) && (this->bitset == rhs.bitset);
    }

    bool operator!=(const const_iterator &rhs) const {
      return not this->operator==(rhs);
    }

    friend const_iterator index_iterable::begin() const;
    friend const_iterator index_iterable::end() const;

  protected:
    std::size_t index;

  private:
    void seek_next() {
      while (++(this->index) < sizeof(T) * CHAR_BIT) {
        if (bitset[this->index] == FLAG) {
          break;
        }
      }
    }

    const std::bitset<sizeof(T) * CHAR_BIT> &bitset;
  };

  explicit index_iterable(const T &value) : bitset{value} {}
  static_assert(std::is_unsigned<T>::value,
                "Only unsigned values are supported");
  static_assert(std::is_integral<T>::value,
                "Only integer values are supported");

  const_iterator begin() const {
    const_iterator iterator{this->bitset};
    iterator.seek_next();

    return iterator;
  }

  const_iterator end() const {
    const_iterator iterator{this->bitset};
    iterator.index = SIZE;

    return iterator;
  }

  bool operator==(const index_iterable &rhs) const {
    return this->bitset == rhs.bitset;
  }

  bool operator!=(const index_iterable &rhs) const {
    return not this->operator==(rhs);
  }

private:
  std::bitset<sizeof(T) * CHAR_BIT> bitset;
};
} // namespace detail

template <typename T> detail::index_iterable<T> make_bit_range(const T &value) {
  return detail::index_iterable<T>{value};
}

template <typename T>
detail::index_iterable<T> make_bit_range(const T &&value) {
  return detail::index_iterable<T>{value};
}

} // namespace util
} // namespace osram

#endif
