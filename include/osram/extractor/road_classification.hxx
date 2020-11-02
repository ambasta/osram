#ifndef OSRAM_EXTRACTOR_ROAD_CLASSIFICATION
#define OSRAM_EXTRACTOR_ROAD_CLASSIFICATION

#include <cassert>
#include <cstdint>
#include <fmt/core.h>

#include <osram/extractor/intersection/constants.hxx>

namespace osram {
namespace extractor {
enum RoadPriorityClass {
  MOTORWAY = 0,
  MOTORWAY_LINK = 1,
  TRUNK = 2,
  TRUNK_LINK = 3,
  PRIMARY = 4,
  PRIMARY_LINK = 5,
  SECONDARY = 6,
  SECONDARY_LINK = 7,
  TERTIARY = 8,
  TERTIARY_LINK = 9,
  MAIN_RESIDENTIAL = 10,
  SIDE_RESIDENTIAL = 11,
  ALLEY = 12,
  PARKING = 13,
  LINK_ROAD = 14,
  UNCLASSIFIED = 15,
  BIKE_PATH = 16,
  FOOT_PATH = 18,
  CONNECTIVITY = 31,
};

class RoadClassification {
  bool motorway_class, link_class, maybe_ignored;
  RoadPriorityClass road_priority_class : RoadPriorityClass::PRIMARY_LINK;
  std::uint8_t number_of_lanes;

public:
  RoadClassification()
      : motorway_class(false), link_class(false), maybe_ignored(false),
        road_priority_class(RoadPriorityClass::CONNECTIVITY),
        number_of_lanes(0) {}

  RoadClassification(bool motorway_class, bool link_class, bool maybe_ignored,
                     RoadPriorityClass road_priority_class,
                     std::uint8_t number_of_lanes)
      : motorway_class(motorway_class), link_class(link_class),
        maybe_ignored(maybe_ignored), road_priority_class(road_priority_class),
        number_of_lanes(number_of_lanes) {}

  bool is_motorway_class() const {
    return motorway_class == true and link_class == false;
  }

  void set_motorway_flag(const bool value) { motorway_class = value; }

  bool is_ramp_class() const { return motorway_class and link_class; };

  bool is_link_class() const { return link_class; }

  void set_link_class(const bool value) { link_class = value; }

  bool is_low_priority_road_class() const { return maybe_ignored; }

  void set_low_priority_flag(const bool value) { maybe_ignored = value; }

  std::uint8_t get_priority() const {
    return static_cast<std::uint32_t>(road_priority_class);
  }

  RoadPriorityClass get_class() const { return road_priority_class; }

  void set_class(const RoadPriorityClass value) { road_priority_class = value; }

  bool operator==(const RoadClassification &other) const {
    return motorway_class == other.motorway_class and
           link_class == other.link_class and
           maybe_ignored == other.maybe_ignored and
           road_priority_class == other.road_priority_class;
  }

  std::string to_string() const {
    return fmt::format("{}{} {} {}", motorway_class ? "motorway" : "normal",
                       link_class ? "_link" : "",
                       maybe_ignored ? "ignorable" : "important",
                       road_priority_class);
  }
};

static_assert(
    sizeof(RoadClassification) == 8,
    "Road Classification should fit eight bytes. Increasing this has a "
    "severe impact on memory.");

inline bool can_be_seen_as_fork(const RoadClassification first,
                                const RoadClassification second) {
  return std::abs(static_cast<int>(first.get_priority()) -
                  static_cast<int>(second.get_priority())) < 2;
}

inline std::uint8_t get_road_group(const RoadClassification classification) {
  const constexpr std::uint8_t groups[RoadPriorityClass::CONNECTIVITY + 1] = {
      0, // MOTORWAY
      0, // MOTORWAY_LINK
      1, // TRUNK
      1, // TRUNK_LINK
      2, // PRIMARY
      2, // PRIMARY_LINK
      2, // SECONDARY
      2, // SECONDARY_LINK
      2, // TERTIARY
      2, // TERTIARY_LINK
      3, // MAIN_RESIDENTIAL
      3, // SIDE_RESIDENTIAL
      3, // ALLEY
      3, // PARKING
      4, // LINK_ROAD
      4, // UNCLASSIFIED
      5, // BIKE_PATH
      6, // FOOT_PATH
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7 // CONNECTIVITY
  };

  assert(groups[RoadPriorityClass::CONNECTIVITY] == 7);
  return groups[classification.get_priority()];
}

inline bool strictly_less(const RoadClassification lhs,
                          const RoadClassification rhs) {
  if (!lhs.is_low_priority_road_class() and rhs.is_low_priority_road_class())
    return false;

  if (lhs.is_low_priority_road_class() and !rhs.is_low_priority_road_class())
    return true;

  return get_road_group(lhs) > get_road_group(rhs);
}

inline bool is_link_to(const RoadClassification link,
                       const RoadClassification road) {
  if (!link.is_link_class() or road.is_link_class())
    return false;

  switch (link.get_priority()) {
  case RoadPriorityClass::MOTORWAY_LINK:
    return road.get_priority() == RoadPriorityClass::MOTORWAY;
  case RoadPriorityClass::TRUNK_LINK:
    return road.get_priority() == RoadPriorityClass::TRUNK;
  case RoadPriorityClass::PRIMARY_LINK:
    return road.get_priority() == RoadPriorityClass::PRIMARY;
  case RoadPriorityClass::SECONDARY_LINK:
    return road.get_priority() == RoadPriorityClass::SECONDARY;
  case RoadPriorityClass::TERTIARY_LINK:
    return road.get_priority() == RoadPriorityClass::TERTIARY;
  default:
    return false;
  }
}

inline bool obvious_by_road_class(const RoadClassification in_classification,
                                  const RoadClassification obvious_candidate,
                                  const RoadClassification compare_candidate) {
  const bool has_high_priority = intersection::PRIORITY_DISTINCTION_FACTOR *
                                     obvious_candidate.get_priority() <
                                 compare_candidate.get_priority();
  const bool continues_on_same_class = in_classification == obvious_candidate;

  return (has_high_priority and continues_on_same_class) or
         (!obvious_candidate.is_low_priority_road_class() and
          !in_classification.is_low_priority_road_class() and
          compare_candidate.is_low_priority_road_class());
}
} // namespace extractor
} // namespace osram
#endif
