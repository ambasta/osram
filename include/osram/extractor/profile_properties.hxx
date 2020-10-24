#ifndef OSRAM_EXTRACTOR_PROFILE_PROPERTIES
#define OSRAM_EXTRACTOR_PROFILE_PROPERTIES
#include "osram/extractor/class_data.hxx"
#include <array>
namespace osram {
namespace extractor {
const constexpr auto DEFAULT_MAX_SPEED = 180 / 3.6; // 180kmph -> m/s

struct ProfileProperties {
  static constexpr int MAX_WEIGHT_NAME_LENGTH = 255,
                       MAX_CLASS_NAME_LENGTH = 255;
  std::int32_t traffic_signal_penalty, u_turn_penalty;

  double max_speed_for_map_matching;
  bool continue_straight_at_waypoint, use_turn_restrictions, left_hand_driving,
      fallback_to_duration, force_split_edges = false,
                            call_tagless_node_function = true;
  char weight_name[MAX_WEIGHT_NAME_LENGTH + 1];
  std::array<char[MAX_CLASS_NAME_LENGTH + 1], MAX_CLASS_INDEX + 1> class_names;
  std::array<ClassData, MAX_EXCLUDABLE_CLASSES> excludable_classes;
  unsigned weight_precision = 1;

  ProfileProperties()
      : traffic_signal_penalty(0), u_turn_penalty(0),
        max_speed_for_map_matching(DEFAULT_MAX_SPEED),
        continue_straight_at_waypoint(true), use_turn_restrictions(false),
        left_hand_driving(false), fallback_to_duration(true),
        weight_name{"duration"}, class_names{{}}, excludable_classes{{}},
        call_tagless_node_function(true) {}

  double get_u_turn_penalty() const { return u_turn_penalty / 10.; }

  void set_u_turn_penalty(const double u_turn_penalty_) {
    u_turn_penalty = static_cast<int>(u_turn_penalty_ * 10.);
  }

  double get_traffic_signal_penalty() const {
    return traffic_signal_penalty / 10.;
  }

  void set_traffic_signal_penalty(const double traffic_signal_penalty_) {
    traffic_signal_penalty = static_cast<int>(traffic_signal_penalty_ * 10.);
  }

  void set_weight_name(std::string_view name) {
    auto count =
        std::min<std::size_t>(name.length(), MAX_WEIGHT_NAME_LENGTH) + 1;
    std::copy_n(name.data(), count, weight_name);
    assert(weight_name[count - 1] == '\0');
    assert(weight_name[MAX_WEIGHT_NAME_LENGTH] == '\0');
    fallback_to_duration = name == "duration";
  }

  std::string get_weight_name() const {
    assert(weight_name[MAX_WEIGHT_NAME_LENGTH] == '\0');
    return std::string{weight_name};
  }

  void set_class_name(std::size_t index, std::string_view name) {
    char *name_ptr = class_names[index];
    auto count =
        std::min<std::size_t>(name.length(), MAX_CLASS_NAME_LENGTH) + 1;
    std::copy_n(name.data(), count, name_ptr);
    assert(class_names[index][count - 1] == '\0');
    assert(class_names[index][MAX_CLASS_NAME_LENGTH] == '\0');
  }

  std::string get_class_name(std::size_t index) const {
    assert(index <= MAX_CLASS_INDEX);
    const auto &name_it = std::begin(class_names) + index;
    return std::string{*name_it};
  }
};
} // namespace extractor
} // namespace osram
#endif
