#ifndef OSRAM_ENGINE_API_BASE_PARAMETERS
#define OSRAM_ENGINE_API_BASE_PARAMETERS

#include <algorithm>
#include <osram/engine/approach.hxx>
#include <osram/engine/bearing.hxx>
#include <osram/engine/hint.hxx>
#include <osram/util/coordinate.hxx>

#include <optional>
#include <vector>

namespace osram {
namespace engine {
namespace api {
struct BaseParameters {
  enum class SnappingType { Default, Any };

  std::vector<util::Coordinate> coordinates;
  std::vector<std::optional<Hint>> hints;
  std::vector<std::optional<double>> radiuses;
  std::vector<std::optional<Bearing>> bearings;
  std::vector<std::optional<Approach>> approaches;
  std::vector<std::string> exclude;

  bool generate_hints = true;

  bool skip_waypoints = false;

  SnappingType snapping = SnappingType::Default;

  BaseParameters(const std::vector<util::Coordinate> coordinates = {},
                 const std::vector<std::optional<Hint>> hints = {},
                 std::vector<std::optional<double>> radiuses = {},
                 std::vector<std::optional<Bearing>> bearings = {},
                 std::vector<std::optional<Approach>> approaches = {},
                 bool generate_hints = true,
                 std::vector<std::string> exclude = {},
                 const SnappingType snapping_ = SnappingType::Default)
      : coordinates(coordinates), hints(hints), radiuses(radiuses),
        bearings(bearings), approaches(approaches), exclude(std::move(exclude)),
        generate_hints(generate_hints), snapping(snapping_) {}

  bool is_valid() const {
    return (hints.empty() || hints.size() == coordinates.size()) and
           (bearings.empty() or bearings.size() == coordinates.size()) and
           (radiuses.empty() or radiuses.size() == coordinates.size()) and
           (approaches.empty() or approaches.size() == coordinates.size()) and
           std::all_of(bearings.begin(), bearings.end(),
                       [](const std::optional<Bearing> &bearing) {
                         if (bearing)
                           return bearing->is_valid();
                         return true;
                       });
  }
};
} // namespace api
} // namespace engine
} // namespace osram
#endif
