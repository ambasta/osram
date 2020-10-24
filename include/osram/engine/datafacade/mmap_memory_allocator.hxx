#ifndef OSRAM_ENGINE_DATAFACADE_MMAP_MEMORY_ALLOCATOR
#define OSRAM_ENGINE_DATAFACADE_MMAP_MEMORY_ALLOCATOR
#include <osram/engine/datafacade/contiguous_block_allocator.hxx>
#include <osram/storage/storage_config.hxx>
#include <vector>

namespace osram {
namespace engine {
namespace datafacade {
class MMapMemoryAllocator : public ContiguousBlockAllocator {
private:
  storage::SharedDataIndex index;
  // to be replaced by mmap_source
  std::vector<std::string> mapped_memory_files;
  std::string rtree_filename;

public:
  explicit MMapMemoryAllocator(const storage::StorageConfig &config);
  ~MMapMemoryAllocator() override final;

  const storage::SharedDataIndex &get_index() override final;
};
} // namespace datafacade
} // namespace engine
} // namespace osram
#endif
