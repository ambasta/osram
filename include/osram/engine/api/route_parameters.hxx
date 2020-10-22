#ifndef OSRAM_ENGINE_API_ROUTE_PARAMETERS
#define OSRAM_ENGINE_API_ROUTE_PARAMETERS
#include <osram/engine/api/base_parameters.hxx>
#include <type_traits>
namespace osram {
namespace engine {
namespace api {
struct RouteParameters : public BaseParameters {
  enum class GeometryType {
    Polyline,
    Polyline6,
    GeoJson,
  };

  enum class OverviewType {
    Simplified,
    Full,
    False,
  };

  enum class AnnotationType {
    None = 0,
    Duration = 0x01,
    Nodes = 0x02,
    Distance = 0x04,
    Weight = 0x08,
    DataSource = 0x10,
    Speed = 0x20,
    All = Duration | Nodes | Distance | Weight | DataSource | Speed
  };

  bool steps = false, alternatives = false, annotations = false;
  unsigned number_of_alternatives = 0;
  AnnotationType annotation_type = AnnotationType::None;
  GeometryType geometry = GeometryType::Polyline;
  OverviewType overview = OverviewType::Simplified;
  std::optional<bool> continue_straight;
  std::vector<std::size_t> waypoints;

  RouteParameters() = default;

  template <typename... Args>
  RouteParameters(const bool steps, const bool alternatives,
                  const AnnotationType annotation_type,
                  const GeometryType geometry, const OverviewType overview,
                  std::optional<bool> continue_straight,
                  std::vector<std::size_t> waypoints, Args... args)
      : BaseParameters{std::forward<Args>(args)...}, steps{steps},
        alternatives{alternatives}, number_of_alternatives{alternatives ? 1u
                                                                        : 0u},
        annotations{annotation_type == AnnotationType::None ? false : true},
        annotation_type{annotation_type}, geometry{geometry},
        overview{overview},
        continue_straight{continue_straight}, waypoints{waypoints} {}

  bool is_valid() const {
    const auto coordinates_ok = coordinates.size() > 1;
    const auto base_parameters_ok = BaseParameters::is_valid();
    const auto waypoints_ok =
        std::all_of(waypoints.begin(), waypoints.end(),
                    [this](const std::size_t &waypoint) {
                      return waypoint < coordinates.size();
                    });
    return coordinates_ok and base_parameters_ok and waypoints_ok;
  }
};

inline bool operator&(RouteParameters::AnnotationType lhs,
                      RouteParameters::AnnotationType rhs) {
  return static_cast<bool>(
      static_cast<std::underlying_type_t<RouteParameters::AnnotationType>>(
          lhs) &
      static_cast<std::underlying_type_t<RouteParameters::AnnotationType>>(
          rhs));
}

inline RouteParameters::AnnotationType
operator|(RouteParameters::AnnotationType lhs,
          RouteParameters::AnnotationType rhs) {
  return (RouteParameters::AnnotationType)(
      static_cast<std::underlying_type_t<RouteParameters::AnnotationType>>(
          lhs) |
      static_cast<std::underlying_type_t<RouteParameters::AnnotationType>>(
          rhs));
}

inline RouteParameters::AnnotationType operator|=(RouteParameters::AnnotationType lhs,
                       RouteParameters::AnnotationType rhs) {
  return lhs = lhs | rhs;
}
} // namespace api
} // namespace engine
} // namespace osram
#endif
