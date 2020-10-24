#ifndef OSRAM_ENGINE_DATAFACADE_PROCESS_MEMORY_ALLOCATOR
#define OSRAM_ENGINE_DATAFACADE_PROCESS_MEMORY_ALLOCATOR

#include <osram/engine/datafacade/contiguous_block_allocator.hxx>
#include <osram/storage/shared_data_index.hxx>
#include <osram/storage/storage_config.hxx>

namespace osram {
namespace engine {
namespace datafacade {
class ProcessMemoryAllocator : public ContiguousBlockAllocator {
private:
  storage::SharedDataIndex index;
  std::unique_ptr<char[]> internal_memory;

public:
  explicit ProcessMemoryAllocator(const storage::StorageConfig &);
  ~ProcessMemoryAllocator() override final;

  const storage::SharedDataIndex &get_index() override final;
};
} // namespace datafacade
} // namespace engine
} // namespace osram
#endif
