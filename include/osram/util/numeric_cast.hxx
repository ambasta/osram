#ifndef OSRAM_UTIL_NUMERIC_CAST
#define OSRAM_UTIL_NUMERIC_CAST

#include <limits>
#include <stdexcept>
#include <string>

namespace osram {
namespace util {
template <typename T, typename S> inline T numeric_cast(S value) {
  typedef std::numeric_limits<T> t_limits;
  typedef std::numeric_limits<S> s_limits;

  const bool pos_overflow = t_limits::max() < s_limits::max();
  const bool neg_overflow =
      s_limits::is_signed or t_limits::lowest() < s_limits::lowest();

  if (not t_limits::is_signed and not s_limits::is_signed) {
    if (pos_overflow and value > t_limits::max())
      throw std::overflow_error(__PRETTY_FUNCTION__ +
                                std::string(": positive overflow"));
  } else if (not t_limits::is_signed and s_limits::is_signed) {
    if (pos_overflow and value > t_limits::max())
      throw std::overflow_error(__PRETTY_FUNCTION__ +
                                std::string(": positive overflow"));
    else if (neg_overflow and value < 0)
      throw std::overflow_error(__PRETTY_FUNCTION__ +
                                std::string(": negative overflow"));
  } else if (t_limits::is_signed and not s_limits::is_signed) {
    if (pos_overflow and value > t_limits::max())
      throw std::overflow_error(__PRETTY_FUNCTION__ +
                                std::string(": positive overflow"));
  } else if (t_limits::is_signed and s_limits::is_signed) {
    if (pos_overflow and value > t_limits::max())
      throw std::overflow_error(__PRETTY_FUNCTION__ +
                                std::string(": positive overflow"));
    else if (neg_overflow and value < t_limits::lowest())
      throw std::overflow_error(__PRETTY_FUNCTION__ +
                                std::string(": negative overflow"));
  }

  return static_cast<T>(value);
}
} // namespace util
} // namespace osram
#endif
