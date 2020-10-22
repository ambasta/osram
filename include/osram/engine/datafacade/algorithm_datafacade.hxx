#ifndef OSRAM_ENGINE_DATAFACADE_ALGORITHM_DATAFACADE
#define OSRAM_ENGINE_DATAFACADE_ALGORITHM_DATAFACADE

#include <osram/engine/algorithm.hxx>

namespace osram {
namespace engine {
namespace datafacade {
using CH = algorithm::ch::Algorithm;
using MLD = algorithm::mld::Algorithm;

template <typename T> class AlgorithmDataFacade;

template <> class AlgorithmDataFacade<CH> {};

template <> class AlgorithmDataFacade<MLD> {};
} // namespace datafacade
} // namespace engine
} // namespace osram
#endif
