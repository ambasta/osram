#ifndef OSRAM_UTIL_PERMUTATION
#define OSRAM_UTIL_PERMUTATION

#include <cstdlib>
#include <iterator>
#include <ranges>
#include <vector>

namespace osram {
namespace util {
template <typename RAIter, typename IndexT>
void inplace_permutation(RAIter begin, RAIter end,
                         const std::vector<IndexT> &old_to_new) {
  std::size_t size = std::distance(begin, end);
  assert(old_to_new.size() == size);

  std::vector<bool> was_replaced(size, false);

  for (auto index : ranges::views::iota(0, (int)size)) {
    if (was_replaced[index])
      continue;

    if (old_to_new[index] == index) {
      was_replaced[index] = true;
      continue;
    }

    auto buffer = begin[index];
    auto old_index = index;
    auto new_index = old_to_new[old_index];

    for (; new_index != index;
         old_index = new_index, new_index = old_to_new[new_index]) {
      was_replaced[old_index] = true;
      std::swap(buffer, begin[new_index]);
    }
    was_replaced[old_index] = true;
    std::swap(buffer, begin[index]);
  }
}
} // namespace util
} // namespace osram
#endif
