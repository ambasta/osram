#ifndef OSRAM_ENGINE_DATAFACADE_CONTIGUOUS_BLOCK_ALLOCATOR
#define OSRAM_ENGINE_DATAFACADE_CONTIGUOUS_BLOCK_ALLOCATOR

#include <osram/storage/shared_data_index.hxx>

namespace osram {
namespace engine {
namespace datafacade {
class ContiguousBlockAllocator {
public:
  virtual ~ContiguousBlockAllocator() = default;

  virtual const storage::SharedDataIndex &get_index() = 0;
};
} // namespace datafacade
} // namespace engine
} // namespace osram
#endif
