#ifndef OSRAM_GUIDANCE_TURN_DATA_CONTAINER
#define OSRAM_GUIDANCE_TURN_DATA_CONTAINER

#include <algorithm>
#include <type_traits>

#include <osram/extractor/travel_mode.hxx>
#include <osram/guidance/turn_bearing.hxx>
#include <osram/guidance/turn_instruction.hxx>

#include <osram/storage/shared_memory_ownership.hxx>
#include <osram/storage/tar_fwd.hxx>

#include <osram/util/typedefs.hxx>
#include <osram/util/vector_view.hxx>

namespace osram {
namespace guidance {
namespace detail {
template <storage::Ownership O> class TurnDataContainerImpl;
}

namespace serialization {
template <storage::Ownership O>
void write(storage::tar::FileWriter &writer, std::string_view name,
           const detail::TurnDataContainerImpl<O> &turn_data);
} // namespace serialization

struct TurnData {
  TurnInstruction turn_instruction;
  LaneDataId lane_data_id;
  EntryClassId entry_class_id;
  TurnBearing pre_turn_bearing, post_turn_bearing;
};

namespace detail {
template <storage::Ownership O> class TurnDataContainerImpl {
  template <typename T> using Vector = util::ViewOrVector<T, O>;

private:
  Vector<TurnInstruction> turn_instructions;
  Vector<LaneDataId> lane_data_ids;
  Vector<EntryClassId> entry_class_ids;
  Vector<TurnBearing> pre_turn_bearings, post_turn_bearings;

public:
  TurnDataContainerImpl() = default;

  TurnDataContainerImpl(Vector<TurnInstruction> turn_instructions,
                        Vector<LaneDataId> lane_data_ids,
                        Vector<EntryClassId> entry_class_ids,
                        Vector<TurnBearing> pre_turn_bearings,
                        Vector<TurnBearing> post_turn_bearings)
      : turn_instructions(std::move(turn_instructions)),
        lane_data_ids(std::move(lane_data_ids)),
        entry_class_ids(std::move(entry_class_ids)),
        pre_turn_bearings(std::move(pre_turn_bearings)),
        post_turn_bearings(std::move(post_turn_bearings)) {}

  EntryClassId get_entry_class_id(const EdgeId edge_id) const {
    return entry_class_ids[edge_id];
  }

  TurnBearing get_pre_turn_bearing(const EdgeId edge_id) const {
    return pre_turn_bearings[edge_id];
  }

  TurnBearing get_post_turn_bearing(const EdgeId edge_id) const {
    return post_turn_bearings[edge_id];
  }

  LaneDataId get_lane_data_id(const EdgeId edge_id) const {
    return lane_data_ids[edge_id];
  }

  bool has_lane_data(const EdgeId edge_id) const {
    return INVALID_LANE_DATAID != lane_data_ids[edge_id];
  }

  guidance::TurnInstruction get_turn_instruction(const EdgeId edge_id) const {
    return turn_instructions[edge_id];
  }

  /*template <typename = std::enable_if<O == storage::Ownership::Container>>
  void push_back(const TurnData &data) {
    turn_instructions.push_back(data.turn_instruction);
    lane_ids.push_back(data.lane_id);
    entry_class_ids.push_back(data.entry_class_id);
    pre_turn_bearings.push_back(data.pre_turn_bearing);
    post_turn_bearings.push_back(data.post_turn_bearing);
  }

  template <typename = std::enable_if<O == storage::Ownership::Container> void
                append(const std::vector<TurnData> &others) {
    std::for_each(others.begin(), others.end(),
                  [this](const TurnData &other) { push_back(other); });
  }*/

  friend void serialization::write<O>(storage::tar::FileWriter &,
                                      std::string_view,
                                      const TurnDataContainerImpl &);
};
} // namespace detail

using TurnDataExternalContainer =
    detail::TurnDataContainerImpl<storage::Ownership::External>;
using TurnDataContainer =
    detail::TurnDataContainerImpl<storage::Ownership::Container>;
using TurnDataView = detail::TurnDataContainerImpl<storage::Ownership::View>;
} // namespace guidance
} // namespace osram
#endif
