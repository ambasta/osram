#include <osram/extractor/extraction_way.hxx>

namespace osram {
namespace extractor {
namespace detail {
inline void maybe_set_string(std::string &str, std::string_view value) {
  if (value == nullptr)
    str.clear();
  else
    str = std::string(value);
}
} // namespace detail
ExtractionWay::ExtractionWay() { clear(); }

void ExtractionWay::clear() {
  forward_speed = -1;
  backward_speed = -1;
  forward_rate = -1;
  backward_rate = -1;
  duration = -1;
  weight = -1;
  name.clear();
  forward_ref.clear();
  backward_ref.clear();
  pronunciation.clear();
  destinations.clear();
  exits.clear();
  turn_lanes_forward.clear();
  turn_lanes_backward.clear();
  road_classification = RoadClassification();
  forward_travel_mode = TRAVEL_MODE_INACCESSIBLE;
  backward_travel_mode = TRAVEL_MODE_INACCESSIBLE;
  roundabout = false;
  circular = false;
  is_startpoint = true;
  forward_restricted = false;
  backward_restricted = false;
  is_left_hand_driving = false;
  highway_turn_classification = 0;
  access_turn_classification = 0;
}

void ExtractionWay::set_name(std::string_view value) {
  detail::maybe_set_string(name, value);
}

std::string_view ExtractionWay::get_name() const { return name; }

void ExtractionWay::set_forward_ref(std::string_view value) {
  detail::maybe_set_string(forward_ref, value);
}

std::string_view ExtractionWay::get_forward_ref() const { return forward_ref; }

void ExtractionWay::set_backward_ref(std::string_view value) {
  detail::maybe_set_string(backward_ref, value);
}

std::string_view ExtractionWay::get_backward_ref() const {
  return backward_ref;
}

void ExtractionWay::set_destinations(std::string_view value) {
  detail::maybe_set_string(destinations, value);
}

std::string_view ExtractionWay::get_destinations() const {
  return destinations;
}

void ExtractionWay::set_exits(std::string_view value) {
  detail::maybe_set_string(exits, value);
}

std::string_view ExtractionWay::get_exits() const { return exits; }

void ExtractionWay::set_pronunciation(std::string_view value) {
  detail::maybe_set_string(pronunciation, value);
}

std::string_view ExtractionWay::get_pronunciation() const {
  return pronunciation;
}

void ExtractionWay::set_turn_lanes_forward(std::string_view value) {
  detail::maybe_set_string(turn_lanes_forward, value);
}

std::string_view ExtractionWay::get_turn_lanes_forward() const {
  return turn_lanes_forward;
}

void ExtractionWay::set_turn_lanes_backward(std::string_view value) {
  detail::maybe_set_string(turn_lanes_backward, value);
}

std::string_view ExtractionWay::get_turn_lanes_backward() const {
  return turn_lanes_backward;
}
} // namespace extractor
} // namespace osram
