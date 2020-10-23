#ifndef OSRAM_ENGINE_DATAFACADE_CONTIGUOUS_INTERNALMEM_DATAFACADE
#define OSRAM_ENGINE_DATAFACADE_CONTIGUOUS_INTERNALMEM_DATAFACADE

#include <osram/engine/algorithm.hxx>
#include <osram/engine/datafacade/algorithm_datafacade.hxx>
#include <osram/engine/datafacade/datafacade_base.hxx>

namespace osram {
namespace engine {
namespace datafacade {
template <typename T> class ContiguousInternalMemoryAlgorithmDataFacade;

template <>
class ContiguousInternalMemoryAlgorithmDataFacade<CH>
    : public datafacade::AlgorithmDataFacade<CH> {};

class ContiguousInternalMemoryDataFacadeBase : public BaseDataFacade {
private:
  std::uint32_t m_checksum;

public:
  ContiguousInternalMemoryDataFacadeBase(
      std::shared_ptr<ContiguousBlockAllocator> allocator,
      const std::string &metric_name, const std::size_t exclude_index)
      : allocator(std::move(allocator)) {
    initialize_internal_pointers(allocator->get_index(), metric_name,
                                 exclude_index);
  }
  std::uint32_t get_checksum() const override final { return m_checksum; }
};

template <typename T> class ContiguousInternalMemoryDataFacade;

template <>
class ContiguousInternalMemoryDataFacade<CH>
    : public ContiguousInternalMemoryDataFacadeBase,
      public ContiguousInternalMemoryAlgorithmDataFacade<CH> {};

template <>
class ContiguousInternalMemoryAlgorithmDataFacade<MLD>
    : public AlgorithmDataFacade<MLD> {};

template <>
class ContiguousInternalMemoryDataFacade<MLD> final
    : public ContiguousInternalMemoryDataFacadeBase,
      public ContiguousInternalMemoryAlgorithmDataFacade<MLD> {};

} // namespace datafacade
} // namespace engine
} // namespace osram
#endif
