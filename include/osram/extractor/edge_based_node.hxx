#ifndef OSRAM_EXTRACTOR_EDGE_BASED_NODE
#define OSRAM_EXTRACTOR_EDGE_BASED_NODE
#include <osram/util/typedefs.hxx>

namespace osram {
    namespace extractor {
        struct EdgeBasedNode {
            GeometryId geometry_id;
            ComponentId component_id;
            std::uint32_t annotation_id;
            bool segregated;
        };
    }
}
#endif
