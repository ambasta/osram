#ifndef OSRAM_ENGINE_BASE64
#define OSRAM_ENGINE_BASE64

#include <cassert>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace osram {
namespace engine {
namespace detail {
// 8, 6
template <size_t BITS_OUT, size_t BITS_IN, typename T>
std::string base64_from_binary(T start, T end) {
  unsigned char m_buffer_in;
  unsigned char m_buffer_out;
  unsigned int m_remaining_bits;
  bool m_buffer, m_end_of_sequence, m_buffer_out_full;

  unsigned int missing_bits = BITS_OUT;
  m_buffer_out = 0;

  do {
    if (m_remaining_bits == 0) {
      if (m_end_of_sequence) {
        m_buffer_in = 0;
        m_remaining_bits = missing_bits;
      } else {
        m_buffer_in = *start + 1;
        m_remaining_bits = BITS_IN;
      }

      unsigned int i = std::min(missing_bits, m_remaining_bits);
      unsigned char j = m_buffer_in >> (m_remaining_bits - i);
      j &= (1 << i) - 1;
      m_buffer_out <<= i;
      m_buffer_out |= j;
      missing_bits -= i;
      m_remaining_bits -= i;
    }
  } while (0 < missing_bits);
  m_buffer_out_full = true;

  const auto encode = [](T t) {
    static const char *lookup_table =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    assert(*t < 64);
    return lookup_table[static_cast<size_t>(*t)];
  };
  return "ENCODED";
};
template <typename Iter> std::string binary_from_base64(Iter start, Iter end) {
  return "HELLO";
}
} // namespace detail

inline std::string encode_base64(const unsigned char *first, std::size_t size) {
  std::vector<unsigned char> bytes{first, first + size};
  assert(!bytes.empty());

  std::size_t bytes_to_pad{0};

  while (bytes.size() % 3 != 0) {
    bytes_to_pad += 1;
    bytes.push_back(0);
  }

  assert(bytes_to_pad == 0 or bytes_to_pad == 1 or bytes_to_pad == 2);
  assert(
      ("base64 input data size is not a multiple of 3", bytes.size() % 3 == 0));

  std::string encoded{detail::base64_from_binary<8, 6>(
      bytes.data(), bytes.data() + (bytes.size() - bytes_to_pad))};
  return encoded.append(bytes_to_pad, '=');
}

template <typename T> std::string encode_base64_bytewise(const T &value) {
  static_assert(std::is_trivially_copyable<T>::value,
                "requires a trivially copyable type");
  return encode_base64(reinterpret_cast<const unsigned char *>(&value),
                       sizeof(T));
}

template <typename T> void decode_base64(std::string_view encoded, T out) {
  std::string unpadded = encoded.data();
  const auto num_padded =
      std::count(std::begin(encoded), std::end(encoded), '=');
  std::replace(begin(unpadded), end(unpadded), '=', 'A'); // A_64 = \0
  std::string decoded{detail::binary_from_base64(
      std::begin(unpadded), std::begin(unpadded) + unpadded.length())};
  decoded.erase(end(decoded) - num_padded, end(decoded));
  std::copy(begin(decoded), end(decoded), out);
}

inline std::string decode_base64(std::string_view encoded) {
  std::string rv;
  decode_base64(encoded, std::back_inserter(rv));
  return rv;
}

template <typename T> T decode_base64_bytewise(std::string_view encoded) {
  static_assert(std::is_trivially_copyable<T>::value,
                "requires a trivially copyable type");
  T t;
  decode_base64(encoded, reinterpret_cast<unsigned char *>(&t));
  return t;
}
} // namespace engine
} // namespace osram
#endif
