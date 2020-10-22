#ifndef OSRAM_UTIL_TYPEDEFS
#define OSRAM_UTIL_TYPEDEFS

#include <cassert>
#include <limits>
#include <osram/util/alias.hxx>

namespace tag {
struct osm_node_id {};
struct osm_way_id {};
struct duplicated_node {};
}; // namespace tag

using OSMNodeId = std::uint64_t;
using OSMWayId = std::uint64_t;
using DuplicatedNodeId = std::uint64_t;
using RestrictionId = std::uint64_t;

static const std::uint64_t SPECIAL_OSM_NODEID =
    std::numeric_limits<std::uint64_t>::max();
static const std::uint64_t SPECIAL_OSM_WAYID = SPECIAL_OSM_NODEID;
static const std::uint64_t MAX_OSM_NODEID = SPECIAL_OSM_NODEID;
static const std::uint64_t MAX_OSM_WAYID = SPECIAL_OSM_NODEID;

static const std::uint64_t MIN_OSM_NODEID =
    std::numeric_limits<std::uint64_t>::min();
static const std::uint64_t MIN_OSM_WAYID = MIN_OSM_NODEID;

using NodeId = std::uint32_t;
using EdgeId = std::uint32_t;
using NameId = std::uint32_t;
using AnnotationId = std::uint32_t;
using EdgeWeight = std::int32_t;
using EdgeDuration = std::int32_t;
using EdgeDistance = float;
using SegmentWeight = std::uint32_t;
using SegmentDuration = std::uint32_t;
using TurnPenalty = std::int16_t;
using DataTimestamp = std::string;

static const std::size_t INVALID_INDEX =
    std::numeric_limits<std::size_t>::max();

using LaneId = std::uint8_t;
static const LaneId INVALID_LANEID = std::numeric_limits<LaneId>::max();
using LaneDataId = std::uint16_t;
static const LaneDataId INVALID_LANE_DATAID =
    std::numeric_limits<LaneDataId>::max();

using LaneDescriptionId = std::uint16_t;
static const LaneDescriptionId INVALID_LANE_DESCRIPTIONID =
    std::numeric_limits<LaneDescriptionId>::max();

using BearingClassId = std::uint32_t;
static const BearingClassId INVALID_BEARING_CLASSID =
    std::numeric_limits<BearingClassId>::max();

using DiscreteBearing = std::uint16_t;

using EntryClassId = std::uint16_t;
static const EntryClassId INVALID_ENTRY_CLASSID =
    std::numeric_limits<EntryClassId>::max();

static const NodeId SPECIAL_NODEID = std::numeric_limits<NodeId>::max();
static const NodeId SPECIAL_SEGMENTID = std::numeric_limits<NodeId>::max() >> 1;
static const NodeId SPECIAL_GEOMETRYID =
    std::numeric_limits<NodeId>::max() >> 1;
static const EdgeId SPECIAL_EDGEID = std::numeric_limits<EdgeId>::max();
static const NameId INVALID_NAMEID = std::numeric_limits<NameId>::max();
static const NameId EMPTY_NAMEID = 0;
static const unsigned INVALID_COMPONENTID = 0;
static const std::size_t SEGMENT_WEIGHT_BITS = 22;
static const std::size_t SEGMENT_DURATION_BITS = 22;
static const SegmentWeight INVALID_SEGMENT_WEIGHT =
    (1u << SEGMENT_WEIGHT_BITS) - 1;
static const SegmentDuration INVALID_SEGMENT_DURATION =
    (1u << SEGMENT_DURATION_BITS) - 1;
static const SegmentWeight MAX_SEGMENT_WEIGHT = INVALID_SEGMENT_WEIGHT - 1;
static const SegmentDuration MAX_SEGMENT_DURATION =
    INVALID_SEGMENT_DURATION - 1;
static const EdgeWeight INVALID_EDGE_WEIGHT =
    std::numeric_limits<EdgeWeight>::max();
static const EdgeDuration MAXIMAL_EDGE_DURATION =
    std::numeric_limits<EdgeDuration>::max();
static const EdgeDistance MAXIMAL_EDGE_DISTANCE =
    std::numeric_limits<EdgeDistance>::max();
static const TurnPenalty INVALID_TURN_PENALTY =
    std::numeric_limits<TurnPenalty>::max();
static const EdgeDistance INVALID_EDGE_DISTANCE =
    std::numeric_limits<EdgeDistance>::max();
static const EdgeDistance INVALID_FALLBACK_SPEED =
    std::numeric_limits<double>::max();

static const EdgeWeight MAXIMAL_EDGE_DURATION_INT_30 = (1 << 29) - 1;

using DataSourceId = std::uint8_t;
using BisectionId = std::uint32_t;
using LevelId = std::uint8_t;
using CellId = std::uint32_t;
using PartitionId = std::uint64_t;

static constexpr auto INVALID_LEVEL_ID = std::numeric_limits<LevelId>::max();
static constexpr auto INVALID_CELL_ID = std::numeric_limits<CellId>::max();

struct SegmentId {
  NodeId id : 31;
  std::uint32_t enabled : 1;

  SegmentId(const NodeId id, const bool enabled) : id{id}, enabled{enabled} {
    assert(!enabled || id != SPECIAL_SEGMENTID);
  }
};

struct GeometryId {
  NodeId id : 31;
  std::uint32_t forward : 1;

  GeometryId()
      : id(std::numeric_limits<unsigned>::max() >> 1), forward(false) {}

  GeometryId(const NodeId id, const bool forward) : id{id}, forward{forward} {}
};

static_assert(sizeof(SegmentId) == 4, "SegmentId needs to be 4 bytes big");

struct ComponentId {
  std::uint32_t id : 31;
  std::uint32_t is_tiny : 1;
};
#endif
