#ifndef OSRAM_UTIL_INDEXED_DATA
#define OSRAM_UTIL_INDEXED_DATA

#include <algorithm>
#include <array>
#include <cassert>

#include <fmt/core.h>
#include <iterator>
#include <limits>
#include <osram/storage/shared_memory_ownership.hxx>
#include <osram/storage/tar_fwd.hxx>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace osram {
namespace util {
namespace detail {
template <typename Policy, storage::Ownership O> struct IndexedDataImpl;
}

namespace serialization {
template <typename Policy, storage::Ownership O>
inline void read(storage::tar::FileReader, std::string_view name,
                 detail::IndexedDataImpl<Policy, O> &index_data);
template <typename Policy, storage::Ownership O>
inline void write(storage::tar::FileWriter, std::string_view name,
                  detail::IndexedDataImpl<Policy, O> &index_data);

template <int N, typename T = std::string> struct VariableGroupBlock {
  static constexpr std::uint32_t BLOCK_SIZE = N;
  using ResultType = T;
  using ValueType = typename T::value_type;

  static_assert(0 <= BLOCK_SIZE and BLOCK_SIZE <= 16, "incorrect block size");
  static_assert(sizeof(ValueType) == 1, "data basic type must be char");

  struct BlockReference {
    std::uint32_t offset, descriptor;
  };

  VariableGroupBlock() {}

  inline std::uint32_t log256(std::uint32_t value) const {
    assert(value < 0x1000000);
    return value == 0 ? 0 : value < 0x100 ? 1 : value < 0x10000 ? 2 : 3;
  }

  template <typename Iter>
  inline void var_advance(Iter &data, Iter &length,
                          std::uint32_t byte_length) const {
    if (byte_length == 1)
      data += static_cast<unsigned char>(*length++);
    else if (byte_length == 2) {
      data += static_cast<unsigned char>(*length++);
      data += static_cast<unsigned char>(*length++) << 8;
    } else if (byte_length != 0) {
      assert(byte_length == 3);
      data += static_cast<unsigned char>(*length++);
      data += static_cast<unsigned char>(*length++) << 8;
      data += static_cast<unsigned char>(*length++) << 16;
    }
  }

  inline std::uint32_t sum_2bits(std::uint32_t value) const {
    value = (value >> 2 & 0x33333333) + (value & 0x33333333);
    value = (value >> 4 & 0x0f0f0f0f) + (value & 0x0f0f0f0f);
    value = (value >> 8 & 0x00ff00ff) + (value & 0x00ff00ff);
    return (value >> 16 & 0x0000ffff) + (value & 0x0000ffff);
  }

  template <typename Offset, typename OffsetIter, typename OutIer>
  OutIer write_block_reference(OffsetIter first, OffsetIter last,
                               Offset &data_offset, OutIer out) const {
    assert(data_offset <=
           std::numeric_limits<decltype(BlockReference::offset)>::max());

    Offset prefix_length = 0;
    BlockReference reference{
        static_cast<decltype(BlockReference::offset)>(data_offset), 0};

    for (; first != last; --last) {
      const std::uint32_t data_length = *last - *std::prev(last);

      if (data_length >= 0x1000000)
        throw std::runtime_error(
            fmt::format("Data length too large: {}", data_length));

      const std::uint32_t byte_length = log256(data_length);
      reference.descriptor = (reference.descriptor << 2) | byte_length;
      prefix_length += byte_length;
    }

    data_offset += prefix_length;
    *out++ = reference;
    return out;
  }

  template <typename Iter, typename OutIter>
  OutIter write_block_prefix(Iter first, Iter last, OutIter out) const {
    for (Iter current = first, next = std::next(first); current != last;
         ++current, ++next) {
      const std::uint32_t data_length = *next - *current;
      const std::uint32_t byte_length = log256(data_length);

      if (byte_length == 0)
        continue;

      out = std::copy_n((const char *)&data_length, byte_length, out);
    }
    return out;
  }

  template <typename Iter>
  void read_referenced_block(const BlockReference &reference,
                             std::uint32_t local_index, Iter &first,
                             Iter &last) const {
    std::uint32_t descriptor = reference.descriptor;
    Iter var_lengths = first;
    std::advance(first, sum_2bits(descriptor));

    for (std::uint32_t idx = 0; idx < local_index; ++idx, descriptor >>= 2)
      var_advance(first, var_lengths, descriptor & 0x3);

    if (local_index < BLOCK_SIZE) {
      last = first;
      var_advance(last, var_lengths, descriptor & 0x03);
    }
  }
};

template <int N, typename T = std::string> struct FixedGroupBlock {
  static constexpr std::uint32_t BLOCK_SIZE = N;

  using ResultType = T;
  using ValueType = typename T::value_type;

  static_assert(sizeof(ValueType) == 1, "data basic type must char");

  struct BlockReference {
    std::uint32_t offset;
  };

  FixedGroupBlock() {}

  template <typename Offset, typename Iter, typename OutIter>
  OutIter write_block_reference(Iter, Iter, Offset &data_offset,
                                OutIter out) const {
    assert(data_offset <=
           std::numeric_limits<decltype(BlockReference::offset)>::max());

    BlockReference reference{
        static_cast<decltype(BlockReference::offset)>(data_offset)};
    data_offset += BLOCK_SIZE;
    *out++ = reference;
    return out;
  }

  template <typename Iter, typename OutIter>
  OutIter write_block_prefix(Iter first, Iter last, OutIter out) const {
    constexpr std::size_t MAX_LENGTH =
        std::numeric_limits<std::make_unsigned_t<ValueType>>::max();
    std::size_t index = 0;
    std::array<ValueType, BLOCK_SIZE> prefix;

    for (Iter current = first, next = std::next(first); current != last;
         ++current, ++next) {
      const std::uint32_t data_length = *next - *current;

      if (data_length > MAX_LENGTH)
        throw std::runtime_error(fmt::format("Data length too large: {} > {}",
                                             data_length, MAX_LENGTH));

      prefix[index++] = data_length;
    }
    out = std::copy_n((const char *)prefix.data(),
                      sizeof(ValueType) * BLOCK_SIZE, out);
    return out;
  }

  template <typename Iter>
  void read_referenced_block(const BlockReference &, std::uint32_t local_index,
                             Iter &first, Iter &last) const {
    Iter fixed_lengths = first;
    std::advance(first, BLOCK_SIZE);

    for (std::uint32_t index = 0; index < local_index; ++index)
      first += static_cast<unsigned char>(*fixed_lengths++);

    if (local_index < BLOCK_SIZE)
      last = first + static_cast<unsigned char>(*fixed_lengths);
  }
};
} // namespace serialization
namespace detail {
template <typename Policy, storage::Ownership O> struct IndexedDataImpl {
public:
  static constexpr std::uint32_t BLOCK_SIZE = Policy::BLOCK_SIZE;

  using BlockNumberType = std::uint32_t;
  using DataSizeType = std::uint64_t;

  using BlockReference = typename Policy::BlockReference;
  using ResultType = typename Policy::ResultType;
  using ValueType = typename Policy::ValueType;

  static_assert(sizeof(ValueType) == 1, "data basic type must char");

private:
  template <class T = ResultType>
  typename std::enable_if<!std::is_same<T, std::string_view>::value, T>::type
  adapt(const ValueType *first, const ValueType *last) const {
    return ResultType(first, last);
  }

  template <class T = ResultType>
  typename std::enable_if<std::is_same<T, std::string_view>::value, T>::type
  adapt(const ValueType *first, const ValueType *last) const {
    return ResultType(first, std::distance(first, last));
  }

  template <typename T> using vector = std::vector<T>;
  vector<BlockReference> blocks;
  vector<ValueType> value;
};
} // namespace detail

template <typename Policy>
using IndexedData =
    detail::IndexedDataImpl<Policy, storage::Ownership::Container>;
template <typename Policy>
using IndexedDataView =
    detail::IndexedDataImpl<Policy, storage::Ownership::View>;
} // namespace util
} // namespace osram
#endif
