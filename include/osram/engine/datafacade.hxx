#ifndef OSRAM_ENGINE_DATAFACADE
#define OSRAM_ENGINE_DATAFACADE
#include <osram/engine/datafacade/contiguous_internalmem_datafacade.hxx>

namespace osram {
namespace engine {
using DataFacadeBase = datafacade::ContiguousInternalMemoryDataFacadeBase;

template <typename T>
using DataFacade = datafacade::ContiguousInternalMemoryDataFacade<T>;
} // namespace engine
} // namespace osram
#endif
