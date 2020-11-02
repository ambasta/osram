#include <cstdio>
#include <cstring>
#include <osram/storage/tar.hxx>

namespace osram {
namespace storage {
namespace tar {
namespace detail {
int MicroTar::open(std::filesystem::path &filepath, std::string_view mode) {
  int err;
  headerT header;
  memset(this, 0, sizeof(*this));

  return err;
}

MicroTar::write(void *data, std::size_t size) {
  unsigned res = fwrite(data, 1, size, stream);
  return (res == size) ? MTAR_ESUCCESS : MTAR_EWRITEFAIL;
}

} // namespace detail
} // namespace tar
} // namespace storage
} // namespace osram
