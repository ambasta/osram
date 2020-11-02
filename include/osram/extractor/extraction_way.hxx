#ifndef OSRAM_EXTRACTOR_EXTRACTION_WAY
#define OSRAM_EXTRACTOR_EXTRACTION_WAY

#include <cstdint>
#include <osram/extractor/road_classification.hxx>
#include <osram/extractor/travel_mode.hxx>

#include <osram/util/guidance/turn_lanes.hxx>
#include <osram/util/typedefs.hxx>
#include <unordered_map>

namespace osram {
namespace extractor {
struct ExtractionWay {
  std::unordered_map<std::string_view, bool> forward_classes, backward_classes;
  double forward_speed, backward_speed, forward_rate, backward_rate, duration,
      weight;

  std::string name, forward_ref, backward_ref, pronunciation, destinations,
      exits, turn_lanes_forward, turn_lanes_backward;

  RoadClassification road_classification;
  TravelMode forward_travel_mode, backward_travel_mode;

  bool roundabout, circular, is_startpoint, forward_restricted,
      backward_restricted, is_left_hand_driving;

  std::uint8_t highway_turn_classification, access_turn_classification;

  ExtractionWay();

  void clear();

  void set_name(std::string_view);

  std::string_view get_name() const;

  void set_forward_ref(std::string_view);

  std::string_view get_forward_ref() const;

  void set_backward_ref(std::string_view);

  std::string_view get_backward_ref() const;

  void set_destinations(std::string_view);

  std::string_view get_destinations() const;

  void set_exits(std::string_view);

  std::string_view get_exits() const;

  void set_pronunciation(std::string_view);

  std::string_view get_pronunciation() const;

  void set_turn_lanes_forward(std::string_view);

  std::string_view get_turn_lanes_forward() const;

  void set_turn_lanes_backward(std::string_view);

  std::string_view get_turn_lanes_backward() const;
};
} // namespace extractor
} // namespace osram
#endif
