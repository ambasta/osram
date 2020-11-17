#ifndef OSRAM_UTIL_MMAP_FILE
#define OSRAM_UTIL_MMAP_FILE

#include <boost/iostreams/device/mapped_file.hpp>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fmt/core.h>

#include <osram/util/vector_view.hxx>

namespace osram {
namespace util {
namespace detail {
template <typename T, typename C>
util::vector_view<T> mmap_file(const std::filesystem::path &file,
                               C &mmap_container) {
  try {
    mmap_container.open(file);
    std::size_t num_objects = mmap_container.size() / sizeof(T);
    auto data_ptr = mmap_container.data();

    assert(reinterpret_cast<uintptr_t>(data_ptr) % alignof(T) == 0);
    return util::vector_view<T>(reinterpret_cast<T *>(data_ptr), num_objects);
  } catch (const std::exception &exc) {
    throw std::runtime_error(
        fmt::format("File {} mapping failed: {}", file, exc.what()));
  }
}

template <typename T, typename C>
util::vector_view<T> mmap_file(const std::filesystem::path &file,
                               C &mmap_container, const std::size_t size) {
  try {
    boost::iostreams::mapped_file_params params;
    params.path = file.string();
    params.flags = boost::iostreams::mapped_file::readwrite;
    params.new_file_size = size;
    mmap_container.open(params);

    std::size_t num_objects = mmap_container.size() / sizeof(T);
    auto data_ptr = mmap_container.data();
    assert(reinterpret_cast<uintptr_t>(data_ptr) % alignof(T) == 0);
    return util::vector_view<T>(reinterpret_cast<T *>(data_ptr), num_objects);
  } catch (const std::exception &exc) {
    throw std::runtime_error(
        fmt::format("File {} mapping failed: {}", file, exc.what()));
  }
}
} // namespace detail

template <typename T>
util::vector_view<const T>
mmap_file(const std::filesystem::path &file,
          boost::iostreams::mapped_file_source &mmap_container) {
  return detail::mmap_file<const T>(file, mmap_container);
}

template <typename T>
util::vector_view<T> mmap_file(const std::filesystem::path &file,
                               boost::iostreams::mapped_file &mmap_container) {
  return detail::mmap_file<T>(file, mmap_container);
}

template <typename T>
util::vector_view<T> mmap_file(const std::filesystem::path &file,
                               boost::iostreams::mapped_file &mmap_container,
                               std::size_t size) {
  return detail::mmap_file<T>(file, mmap_container, size);
}
} // namespace util
} // namespace osram
#endif
