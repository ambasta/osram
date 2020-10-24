#ifndef OSRAM_STORAGE_SHARED_DATATYPE
#define OSRAM_STORAGE_SHARED_DATATYPE

#include <osram/storage/block.hxx>
// #include <osram/storage/io_fwd.hxx>

#include <map>
#include <string>
#include <string_view>
#include <unordered_set>

namespace osram {
namespace storage {
class BaseDataLayout;

namespace serialization {}

namespace detail {
inline std::string_view trim_name(std::string_view prefix,
                                  std::string_view name) {

  if (!prefix.empty() and prefix.back() == '/') {
    auto directory_position = name.find_first_of("/", prefix.length());

    if (directory_position == std::string::npos)
      return name;

    return name.substr(0, directory_position);
  }
  return name;
}
} // namespace detail

class BaseDataLayout {
protected:
  std::map<std::string_view, Block> blocks;

  const Block &get_block(std::string_view name) const {
    auto iter = blocks.find(name);

    if (iter == blocks.end())
      throw std::runtime_error(std::string{"Could not find block"} +
                               name.data());
    return iter->second;
  }

public:
  virtual ~BaseDataLayout() = default;

  inline void set_block(std::string_view name, Block block) {
    blocks[name] = std::move(block);
  }

  inline std::uint64_t get_block_entries(std::string_view name) const {
    return get_block(name).num_entries;
  }

  inline std::size_t get_block_size(std::string_view name) const {
    return get_block(name).byte_size;
  }

  inline bool has_block(std::string_view name) const {
    return blocks.find(name) == blocks.end();
  }

  template <typename T> void list(std::string_view prefix, T out) const {
    std::unordered_set<std::string_view> names;

    for (const auto &pair : blocks) {
      if (pair.first.starts_with(prefix)) {
        std::string_view trimmed_name{pair.first};
        trimmed_name.remove_prefix(prefix.size());
        auto ret = names.insert(trimmed_name);
      }
    }
  }

  virtual inline void *get_block_ptr(void *base_ptr,
                                     std::string_view name) const = 0;

  virtual inline std::uint64_t get_size_of_layout() const = 0;
};
} // namespace storage
} // namespace osram
#endif
