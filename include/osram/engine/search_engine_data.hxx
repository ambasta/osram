#ifndef OSRAM_ENGINE_SEARCH_ENGINE_DATA
#define OSRAM_ENGINE_SEARCH_ENGINE_DATA

#include <osram/engine/algorithm.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace engine {
template <typename T> struct SearchEngineData {};

struct HeapData {
  NodeId parent;
  HeapData(NodeId p) : parent(p) {}
};

template <> struct SearchEngineData<algorithm::ch::Algorithm> {};
} // namespace engine
} // namespace osram

#endif
