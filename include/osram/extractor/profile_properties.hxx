#ifndef OSRAM_EXTRACTOR_PROFILE_PROPERTIES
#define OSRAM_EXTRACTOR_PROFILE_PROPERTIES
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <osram/extractor/class_data.hxx>

namespace osram {
namespace extractor {
const constexpr auto DEFAULT_MAX_SPEED = 100 / 3.6; // 100 kmph - > m/s

struct ProfileProperties {
  static constexpr int MAX_WEIGHT_NAME_LENGTH = 255,
                       MAX_CLASS_NAME_LENGTH = 255;

  std::int32_t traffic_signal_penalty, u_turn_penalty;

  double max_speed_for_map_matching;

  bool continue_straight_at_waypoint, use_turn_restriction, left_hand_driving,
      fallback_to_duration, force_split_edges = false,
                            call_tagless_node_function = true;

  std::string weight_name;

  std::array<std::string, MAX_CLASS_INDEX + 1> class_names;

  std::array<ClassData, MAX_EXCLUDABLE_CLASSES> excludable_classes;

  unsigned weight_precision = 1;

  ProfileProperties();

  double get_u_turn_penalty() const;

  void set_u_turn_penalty(const double);

  double get_traffic_signal_penalty() const;

  void set_traffic_signal_penalty(const double);

  double get_max_speed_for_map_matching() const;

  void set_max_speed_for_map_matching(const double);

  void set_weight_name(std::string_view);

  std::string get_weight_name() const;

  void set_excludable_classes(std::size_t, ClassData);

  std::optional<std::size_t> classes_are_excludable(ClassData) const;

  void set_class_name(std::size_t, std::string_view);

  std::string get_class_name(std::size_t) const;

  double get_weight_multiplier() const;

  double get_max_turn_weight() const;
};
} // namespace extractor
} // namespace osram
#endif
