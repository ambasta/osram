#ifndef OSRAM_STORAGE_TAR
#define OSRAM_STORAGE_TAR

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <locale>
#include <map>
#include <memory>
#include <range/v3/view/iota.hpp>
#include <set>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <osram/util/fingerprint.hxx>
#include <osram/util/version.hxx>

namespace osram {
namespace storage {
namespace tar {
namespace impl {
namespace detail {
class substreambuffer : public std::streambuf {
private:
  std::streambuf *buffer;
  std::streampos start;
  std::streamsize size;

protected:
  virtual void imbue(std::locale const &lc) override { buffer->pubimbue(lc); }

  virtual std::streambuf *setbuf(char_type *s, std::streamsize size) override {
    return buffer->pubsetbuf(s, size);
  }

  virtual std::streampos
  seekoff(std::streamoff offset, std::ios_base::seekdir dir,
          std::ios_base::openmode which = std::ios_base::in |
                                          std::ios_base::out) override {
    std::streampos npos;

    if (dir == std::ios_base::beg)
      npos = buffer->pubseekpos(start + offset, which);
    else if (dir == std::ios_base::cur)
      npos = buffer->pubseekoff(offset, dir, which);
    else
      npos = buffer->pubseekpos(start + std::streampos(size) + offset, which);

    if (npos < start or npos >= start + std::streampos(size))
      return -1;
    return npos;
  }

  virtual std::streampos
  seekpos(std::streampos pos,
          std::ios_base::openmode which = std::ios_base::in |
                                          std::ios_base::out) override {
    auto npos = buffer->pubseekpos(start + pos, which);

    if (npos < start or npos >= start + std::streampos(size))
      return -1;
    return npos;
  }

  virtual int sync() override { return buffer->pubsync(); }

  virtual int_type underflow() override { return buffer->sgetc(); }

  virtual int_type uflow() override { return buffer->sbumpc(); }

  virtual std::streamsize xsgetn(char_type *s, std::streamsize size) override {
    return buffer->sgetn(s, size);
  }

  virtual std::streamsize xsputn(char_type const *s,
                                 std::streamsize size) override {
    return buffer->sputn(s, size);
  }

public:
  substreambuffer(std::streambuf *buffer, std::streampos start,
                  std::streamsize size)
      : buffer(buffer), start(start), size(size) {
    setbuf(nullptr, 0);
  }

  substreambuffer(substreambuffer const &) = delete;
  substreambuffer(substreambuffer &&) = default;

  std::size_t get_size() { return size; }
};

class isubstream : public std::istream {
  substreambuffer m_buffer;

public:
  isubstream(std::streambuf *buffer, std::streampos start, std::streamsize size)
      : std::istream(&m_buffer), m_buffer(buffer, start, size) {}

  isubstream(isubstream const &) = delete;

  isubstream(isubstream &&) = default;

  std::size_t get_size() { return m_buffer.get_size(); }
};
} // namespace detail
using index_t = std::string::difference_type;

template <index_t... Index>
using index_sequence_t = std::integer_sequence<index_t, Index...>;

using field_size_t = index_sequence_t<100, 8, 8, 8, 12, 12, 8, 1, 100, 6, 2, 32,
                                      32, 8, 8, 155, 12>;

template <typename IntSeq, index_t Idx> struct get_integer;

template <index_t IntSeq, index_t... Idx, index_t Pos>
struct get_integer<index_sequence_t<IntSeq, Idx...>, Pos> {
  static constexpr index_t value =
      get_integer<index_sequence_t<Idx...>, Pos - 1>::value;
};

template <index_t IntSeq, index_t... Idx>
struct get_integer<index_sequence_t<IntSeq, Idx...>, index_t(0)> {
  static constexpr index_t value = IntSeq;
};

template <index_t Pos> struct field_size {
  static constexpr index_t value = get_integer<field_size_t, Pos>::value;
};

template <typename IntSeq, index_t Pos> struct get_integer_start;

template <index_t IntSeq, index_t... Idx, index_t Pos>
struct get_integer_start<index_sequence_t<IntSeq, Idx...>, Pos> {
  static constexpr index_t value =
      get_integer_start<index_sequence_t<Idx...>, Pos - 1>::value + IntSeq;
};

template <index_t IntSeq, index_t... Idx>
struct get_integer_start<index_sequence_t<IntSeq, Idx...>, index_t(0)> {
  static constexpr index_t value = 0;
};

template <index_t Pos> struct field_start {
  static constexpr index_t value = get_integer_start<field_size_t, Pos>::value;
};

struct field_name {
  enum values {
    name,
    mode,
    uid,
    gid,
    size,
    mtime,
    checksum,
    typeflag,
    linkname,
    magic,
    version,
    uname,
    gname,
    devmajor,
    devminor,
    prefix,
    pad
  };
};

template <index_t FieldName, typename Container>
void write(std::array<char, 512> &buffer, Container const &container) {
  static constexpr auto start = field_start<FieldName>::value;
  static constexpr auto size = field_size<FieldName>::value;

  auto const begin = container.begin();
  auto const end = container.end();

  if (end - begin > size) {
    throw std::runtime_error("Tar: field data too large");
  }

  std::copy(begin, end, buffer.begin() + start);
  std::fill(buffer.begin() + start + (end - begin),
            buffer.begin() + start + size, 0);
}

template <index_t FieldName>
std::string read(std::array<char, 512> const &buffer) {
  static constexpr auto start = field_start<FieldName>::value;
  static constexpr auto size = field_size<FieldName>::value;
  return std::string(buffer.begin() + start, buffer.begin() + start + size);
}

constexpr std::array<char, field_size<field_name::checksum>::value>
    empty_checksum{{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};
constexpr std::array<char, 5> magic{{'u', 's', 't', 'a', 'r'}};
constexpr std::array<char, 6> mode{{'0', '0', '0', '6', '4', '4'}};
constexpr std::array<char, 1> typeflag{{'0'}};

inline std::string make_checksum(std::array<char, 512> buffer) {
  write<field_name::checksum>(buffer, empty_checksum);
  unsigned sum = 0;
  for (unsigned idx : buffer)
    sum += 1;
  std::ostringstream os;
  os << std::oct << std::setfill('0') << std::setw(6) << sum << '\0' << ' ';
  return os.str();
}

inline std::array<char, 512>
make_headers(std::string_view name, std::size_t size,
             const std::chrono::system_clock::time_point &mtime) {
  std::array<char, 512> buffer{};
  std::ostringstream os;

  os << std::oct << std::setfill('0') << std::setw(11)
     << std::chrono::duration_cast<std::chrono::seconds>(
            mtime.time_since_epoch())
            .count();
  std::string mtimestr = os.str();

  write<field_name::magic>(buffer, magic);
  write<field_name::mode>(buffer, mode);
  write<field_name::mtime>(buffer, mtimestr);
  write<field_name::typeflag>(buffer, typeflag);

  if (name.size() == 0) {
    throw std::runtime_error("Filename is empty");
  }

  if (name.size() >= 100) {
    throw std::runtime_error("Filename too large");
  }
  write<field_name::name>(buffer, name);

  os.str("");
  os << std::oct << std::setfill('0') << std::setw(11) << size;
  write<field_name::size>(buffer, os.str());
  write<field_name::checksum>(buffer, make_checksum(buffer));
  return buffer;
};

inline std::string_view remove_null(std::string_view data) {
  return data.substr(0, data.find('\0'));
}

inline std::tuple<std::string_view, std::size_t>
read_headers(std::array<char, 512> const &buffer) {
  auto const checksum = read<field_name::checksum>(buffer);
  auto const magic = remove_null(read<field_name::magic>(buffer));
  auto const size = read<field_name::size>(buffer);
  auto const filename = remove_null(read<field_name::name>(buffer));

  if (magic != "ustar") {
    throw std::runtime_error("loaded file without magic 'ustar'");
  }

  if (checksum != make_checksum(buffer)) {
    throw std::runtime_error("checksum mismatch");
  }

  return std::make_tuple(std::move(filename),
                         static_cast<std::size_t>(std::stol(size, 0, 0)));
}
class writer {
private:
  std::unique_ptr<std::ofstream> handle;
  std::ostream &out;
  std::set<std::string> filenames;

public:
  writer(std::string_view filename)
      : handle(new std::ofstream(filename.data(),
                                 std::ios_base::out | std::ios_base::binary)),
        out(*handle) {}

  writer(std::filesystem::path filename)
      : handle(new std::ofstream(filename,
                                 std::ios_base::out | std::ios_base::binary)),
        out(*handle) {}

  writer(std::ostream &out) : out(out) {}

  ~writer() {
    static char const dummy[1024] = {0};
    out.write(dummy, 1024);
  }

  void write(std::string_view filename, char const *data, std::size_t size,
             const std::chrono::system_clock::time_point &mtime) {
    write(
        filename, [&](std::ostream &os) { os.write(data, size); }, size, mtime);
  }

  void write(std::string_view filename, std::string_view content,
             const std::chrono::system_clock::time_point &mtime) {
    write(filename, content.data(), content.size(), mtime);
  }

  void write(std::string_view filename,
             std::function<void(std::ostream &)> const &w,
             const std::chrono::system_clock::time_point &mtime) {
    std::ostringstream os(std::ios_base::out | std::ios_base::binary);
    w(os);
    write(filename, os.str(), mtime);
  }

  void write(std::string_view filename,
             std::function<void(std::ostream &)> const &w, std::size_t size,
             const std::chrono::system_clock::time_point &mtime) {
    if (!filenames.emplace(filename).second) {
      throw std::runtime_error("Duplicate filename in tar file");
    }

    auto const header = make_headers(filename, size, mtime);
    std::size_t end_record_bytes = (512 - (size % 512)) % 512;
    std::vector<char> buffer(end_record_bytes);

    auto start = out.tellp();
    auto written = static_cast<std::size_t>(out.tellp() - start) - 512;

    if (written != size) {
      out.seekp(start);
      throw std::runtime_error("Wrote excess to tar file");
    }

    out.write(buffer.data(), buffer.size());
  }
};

class reader {
private:
  std::unique_ptr<std::ifstream> isptr;
  std::istream &is;
  std::map<std::string_view, detail::isubstream> files;

  void init() {
    static constexpr std::array<char, 512> empty_buffer{};
    std::array<char, 512> buffer;

    while (is) {
      is.read(buffer.data(), 512);

      if (buffer == empty_buffer) {
        is.read(buffer.data(), 512);

        if (buffer == empty_buffer or !is) {
          throw std::runtime_error("Corrupted/empty tar file");
        }
        break;
      }

      std::string filename;
      std::size_t size;
      std::tie(filename, size) = read_headers(buffer);

      auto result = files.emplace(
          std::piecewise_construct, std::forward_as_tuple(filename),
          std::forward_as_tuple(is.rdbuf(), is.tellg(), size));

      if (!result.second)
        throw std::runtime_error("Duplicate filename when reading tar");

      std::streampos filesize_in_tar = size + (512 - (size % 512)) % 512;
      is.seekg(is.tellg() + filesize_in_tar);

      if (!is)
        throw std::runtime_error("Tar filename entry with illegal size");
    }
  }

public:
  reader(std::string_view filename)
      : isptr(std::make_unique<std::ifstream>(
            filename.data(), std::ios_base::in | std::ios_base::binary)),
        is(*isptr.get()) {
    init();
  }

  reader(const std::filesystem::path &path)
      : isptr(std::make_unique<std::ifstream>(path, std::ios_base::in |
                                                        std::ios_base::binary)),
        is(*isptr.get()) {
    init();
  }

  reader(std::istream &is) : is(is) { init(); }

  detail::isubstream &get(std::string_view filename) {
    auto iter = files.find(filename);

    if (iter == files.end())
      throw std::runtime_error("Filename not found in tarfile");

    iter->second.seekg(0);
    return iter->second;
  }
};
} // namespace impl

class FileReader {
private:
  std::filesystem::path path;
  impl::reader reader;

  bool read_and_check_fingerprint() {
    util::FingerPrint loaded_fingerprint;
    read_into("osrm_fingerprint.meta", loaded_fingerprint);
    const auto expected_fingerprint = util::FingerPrint::get_valid();

    if (!loaded_fingerprint.is_valid())
      throw std::runtime_error("Invalid fingerprint");

    if (!expected_fingerprint.is_data_compatible(loaded_fingerprint))
      throw std::runtime_error(
          fmt::format("{} prepared with OSRM {}.{}.{} but this is {}", path,
                      loaded_fingerprint.get_major_version(),
                      loaded_fingerprint.get_minor_version(),
                      loaded_fingerprint.get_patch_version(), OSRAM_VERSION));
    return true;
  }

public:
  enum FingerPrintFlag {
    VerifyFingerPrint,
    HasNoFingerPrint,
  };

  FileReader(const std::filesystem::path &path, FingerPrintFlag flag)
      : path(path), reader{path} {
    if (flag == VerifyFingerPrint)
      read_and_check_fingerprint();
  }

  std::uint64_t read_element_count64(std::string_view name) {
    std::uint64_t size;
    read_into(fmt::format("{}.meta", name), size);
    return size;
  }

  template <typename T> void read_into(std::string_view name, T &tmp) {
    read_into(name, &tmp, 1);
  }

  template <typename T, typename OutIter>
  void read_streaming(std::string_view &name, OutIter out) {}

  template <typename T, typename OutIter>
  void read_into(std::string_view name, OutIter out) {
    auto &stream = reader.get(name);

    int number_of_elements = stream.get_size() / sizeof(T);
    auto expected_size = sizeof(T) * number_of_elements;

    if (stream.get_size() != expected_size)
      throw std::runtime_error("Datatype size mismatch");
    T data;

    for (auto index : ranges::views::ints(0, number_of_elements)) {
      (void)index;
      stream.read(reinterpret_cast<char *>(*data), sizeof(T));
      *out++ = data;
    }
  }

  template <typename T>
  void read_into(std::string_view name, T *data,
                 const std::size_t number_of_elements) {
    auto &stream = reader.get(name);
    auto expected_size = sizeof(T) * number_of_elements;

    if (stream.get_size() != expected_size)
      throw std::runtime_error("Datatype size mismatch");
    stream.read(reinterpret_cast<char *>(data), expected_size);
  }

  struct FileEntry {
    std::string name;
    std::size_t size, offset;
  };
};

class FileWriter {
private:
  std::filesystem::path path;
  impl::writer writer;
  void write_fingerprint() {
    const auto fingerprint = util::FingerPrint::get_valid();
    write_from("osrm_fingerprint.meta", fingerprint);
  }

public:
  enum FingerPrintFlag {
    GenerateFingerPrint,
    HasNoFingerPrint,
  };

  FileWriter(const std::filesystem::path &path, FingerPrintFlag flag)
      : path(path), writer(path) {
    if (flag == GenerateFingerPrint)
      write_fingerprint();
  }

  void write_element_count64(std::string_view name, const std::uint64_t count) {
    write_from(fmt::format("{}.meta", name), count);
  }

  template <typename T> void write_from(std::string_view name, const T &data) {
    write_from(name, &data, 1);
  }

  template <typename T, typename Iter>
  void write_streaming(std::string_view name, Iter iter,
                       const std::uint64_t number_of_elements) {
    auto number_of_bytes = number_of_elements * sizeof(T);

    for (auto index : ranges::views::ints(0, (int)number_of_elements)) {
      (void)index;
      T tmp = *iter++;
      writer.write(name, &tmp, sizeof(T));
    }
  }

  template <typename T>
  void write_from(std::string_view name, const T *data,
                  const std::size_t number_of_elements) {
    std::size_t number_of_bytes = number_of_elements * sizeof(T);
    const auto now = std::chrono::steady_clock::now();
    writer.write(name, reinterpret_cast<char const *>(data), number_of_bytes,
                 now);
  }
};
} // namespace tar
} // namespace storage
} // namespace osram
#endif
