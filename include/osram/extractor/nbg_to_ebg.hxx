#ifndef OSRAM_EXTRACTOR_NBG_TO_EBG
#define OSRAM_EXTRACTOR_NBG_TO_EBG
#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
struct NBGToEBG {
  NodeId source, target, forward_ebg_node, backward_ebg_node;
};
} // namespace extractor
} // namespace osram

#endif
