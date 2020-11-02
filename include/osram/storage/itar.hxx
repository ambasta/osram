#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <locale>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace tar {
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
  seekoff(std::streamoff offset, std::ios_base::seek_dir dir,
          std::ios_base::open_mode which = std::ios_base::in |
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
          std::ios_base::open_mode which = std::ios_base::in |
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
};

class isubstream : public std::istream {
private:
  substreambuffer m_buffer;

public:
  isubstream(std::streambuf *buffer, std::streampos start, std::streamsize size)
      : std::istream(&m_buffer), m_buffer(buffer, start, size) {}

  isubstream(isubstream const &) = delete;

  isubstream(isubstream &&) = default;
};
} // namespace detail
namespace impl {
namespace tar {
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

inline std::array<char, 512> make_headers(std::string_view name,
                                          std::size_t size) {
  std::array<char, 512> buffer{};
  std::ostringstream os;

  os << std::oct << std::setfill('0') << std::setw(11) << std::time(nullptr);
  std::string mtime = os.str();

  write<field_name::magic>(buffer, magic);
  write<field_name::mode>(buffer, mode);
  write<field_name::mtime>(buffer, mtime);
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
} // namespace tar
} // namespace impl
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

  writer(std::ostream &out) : out(out) {}

  ~writer() {
    static char const dummy[1024] = {0};
    out.write(dummy, 1024);
  }

  void write(std::string_view filename, char const *data, std::size_t size) {
    write(
        filename, [&](std::ostream &os) { os.write(data, size); }, size);
  }

  void write(std::string_view filename, std::string_view content) {
    write(filename, content.data(), content.size());
  }

  void write(std::string_view filename,
             std::function<void(std::ostream &)> const &w) {
    std::ostringstream os(std::ios_base::out | std::ios_base::binary);
    w(os);
    write(filename, os.str());
  }

  void write(std::string_view filename,
             std::function<void(std::ostream &)> const &w, std::size_t size) {
    if (!filenames.emplace(filename).second) {
      throw std::runtime_error("Duplicate filename in tar file");
    }

    auto const header = impl::tar::make_headers(filename, size);
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
      std::tie(filename, size) = impl::tar::read_headers(buffer);

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

  reader(std::istream &is) : is(is) { init(); }

  std::istream &get(std::string_view filename) {
    auto iter = files.find(filename);

    if (iter == files.end())
      throw std::runtime_error("Filename not found in tarfile");

    iter->second.seekg(0);
    return iter->second;
  }
};
} // namespace tar
