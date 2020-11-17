#include <cmath>
#include <osram/extractor/profile_properties.hxx>
#include <osram/util/numeric_cast.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
ProfileProperties::ProfileProperties()
    : traffic_signal_penalty(0), u_turn_penalty(0),
      max_speed_for_map_matching(DEFAULT_MAX_SPEED),
      continue_straight_at_waypoint(true), use_turn_restriction(false),
      left_hand_driving(false), fallback_to_duration(true),
      call_tagless_node_function(true), weight_name{"duration"},
      class_names{{}}, excludable_classes{{}} {}

double ProfileProperties::get_u_turn_penalty() const {
  return u_turn_penalty / 10.;
}

void ProfileProperties::set_u_turn_penalty(const double penalty) {
  u_turn_penalty = util::numeric_cast<int>(penalty * 10.);
}

double ProfileProperties::get_traffic_signal_penalty() const {
  return traffic_signal_penalty / 10.;
}

void ProfileProperties::set_traffic_signal_penalty(const double penalty) {
  traffic_signal_penalty =
      util::numeric_cast<int>(traffic_signal_penalty * 10.);
}

double ProfileProperties::get_max_speed_for_map_matching() const {
  return max_speed_for_map_matching;
}

void ProfileProperties::set_max_speed_for_map_matching(const double speed) {
  max_speed_for_map_matching = speed;
}

void ProfileProperties::set_weight_name(std::string_view name) {
  auto idx = std::min<std::size_t>(name.length(), MAX_WEIGHT_NAME_LENGTH);
  weight_name = std::string{name.substr(0, idx).data()};
  fallback_to_duration = name == "duration";
}

std::string ProfileProperties::get_weight_name() const { return weight_name; }

void ProfileProperties::set_excludable_classes(std::size_t index,
                                               ClassData classes) {
  excludable_classes[index] = classes;
}

std::optional<std::size_t>
ProfileProperties::classes_are_excludable(ClassData classes) const {
  auto iter = std::find(excludable_classes.cbegin(), excludable_classes.cend(),
                        classes);

  if (iter != excludable_classes.cend())
    return std::distance(excludable_classes.begin(), iter);

  return {};
}

void ProfileProperties::set_class_name(std::size_t index,
                                       std::string_view name) {
  auto idx = std::min<std::size_t>(name.length(), MAX_CLASS_NAME_LENGTH);
  class_names[index] = std::string{name.substr(0, idx).data()};
}

std::string ProfileProperties::get_class_name(std::size_t index) const {
  return std::string(class_names[index]);
}

double ProfileProperties::get_weight_multiplier() const {
  return std::pow(10., weight_precision);
}

double ProfileProperties::get_max_turn_weight() const {
  return std::numeric_limits<TurnPenalty>::max() / get_weight_multiplier();
}
} // namespace extractor
} // namespace osram
