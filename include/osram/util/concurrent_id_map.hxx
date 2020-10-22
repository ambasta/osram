#ifndef OSRAM_UTIL_CONCURRENT_ID_MAP
#define OSRAM_UTIL_CONCURRENT_ID_MAP

#include <functional>
#include <shared_mutex>
#include <type_traits>
#include <unordered_map>

namespace osram {
namespace util {
template <typename K, typename V, typename H = std::hash<K>>
struct ConcurrentIdMap {
  static_assert(std::is_unsigned<V>::value,
                "Only unsigned integer types are supported.");

  using UpgradableMutex = std::shared_mutex;
  using ScopedReaderLock = std::shared_lock<UpgradableMutex>;
  using ScopedWriterLock = std::unique_lock<UpgradableMutex>;

  std::unordered_map<K, V, H> data;
  mutable UpgradableMutex mutex;

  ConcurrentIdMap() = default;

  ConcurrentIdMap(ConcurrentIdMap &&other) {
    if (this != &other) {
      ScopedWriterLock other_lock{other.mutex};
      ScopedWriterLock lock{mutex};

      data = std::move(other.data);
    }
  }

  ConcurrentIdMap &operator=(ConcurrentIdMap &&other) {
    if (this != &other) {
      ScopedWriterLock other_lock{other.mutex};
      ScopedWriterLock lock{mutex};
      data = std::move(other.data);
    }
    return *this;
  }

  const V find_or_add(const K &key) {
    {
      ScopedReaderLock sentry{mutex};
      const auto result = data.find(key);
      if (result != data.end())
        return result->second;
    }
    {
      ScopedWriterLock sentry{mutex};
      const auto result = data.find(key);

      if (result != data.end())
        return result->second;
      const auto id = static_cast<V>(data.size());
      data[key] = id;
      return id;
    }
  }
};
} // namespace util
} // namespace osram
#endif
