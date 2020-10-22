#ifndef OSRAM_ENGINE_API_TABLE_PARAMETERS
#define OSRAM_ENGINE_API_TABLE_PARAMETERS

#include "osram/util/typedefs.hxx"
#include <osram/engine/api/base_parameters.hxx>

namespace osram {
namespace engine {
namespace api {
struct TableParameters : public BaseParameters {
  std::vector<size_t> sources, targets;
  double fallback_speed = INVALID_FALLBACK_SPEED;

  enum class FallbackCoordinateType {
    Input = 0,
    Snapped = 1,
  };

  FallbackCoordinateType fallback_coordinate_type =
      FallbackCoordinateType::Input;

  enum class AnnotationsType {
    None = 0,
    Duration = 0x01,
    Distance = 0x02,
    ALL = Duration | Distance,
  };

  AnnotationsType annotations = AnnotationsType::Duration;

  double scale_factor = 1;

  TableParameters() = default;

  template <typename... Args>
  TableParameters(std::vector<size_t> sources, std::vector<size_t> targets,
                  Args... args)
      : BaseParameters{std::forward<Args>(args)...},
        sources{std::move(sources)}, targets{std::move(targets)} {}

  template <typename... Args>
  TableParameters(std::vector<size_t> sources, std::vector<size_t> targets,
                  const AnnotationsType annotations, Args... args)
      : BaseParameters{std::forward<Args>(args)...}, sources{std::move(
                                                         sources)},
        targets{std::move(targets)}, annotations{annotations} {}

  template <typename... Args>
  TableParameters(std::vector<size_t> sources, std::vector<size_t> targets,
                  const AnnotationsType annotations, double fallback_speed,
                  FallbackCoordinateType fallback_coordinate_type,
                  double scale_factor, Args... args)
      : BaseParameters{std::forward<Args>(args)...},
        sources{std::move(sources)}, targets{std::move(targets)},
        annotations{annotations}, fallback_speed{fallback_speed},
        fallback_coordinate_type{fallback_coordinate_type}, scale_factor{
                                                                scale_factor} {}

  bool is_valid() const {
    if (!BaseParameters::is_valid())
      return false;

    if (coordinates.size() < 2)
      return false;

    const auto not_in_range = [this](const std::size_t &index) {
      return index >= coordinates.size();
    };

    if (std::any_of(sources.begin(), sources.end(), not_in_range))
      return false;

    if (std::any_of(targets.begin(), targets.end(), not_in_range))
      return false;

    if (fallback_speed < 1 or scale_factor < 1)
      return false;

    return true;
  }
};

inline bool operator&(TableParameters::AnnotationsType lhs,
                      TableParameters::AnnotationsType rhs) {
  return static_cast<bool>(
      static_cast<std::underlying_type_t<TableParameters::AnnotationsType>>(
          lhs) &
      static_cast<std::underlying_type_t<TableParameters::AnnotationsType>>(
          rhs));
}

inline TableParameters::AnnotationsType
operator|(TableParameters::AnnotationsType lhs,
          TableParameters::AnnotationsType rhs) {
  return (TableParameters::AnnotationsType)(
      static_cast<std::underlying_type_t<TableParameters::AnnotationsType>>(
          lhs) |
      static_cast<std::underlying_type_t<TableParameters::AnnotationsType>>(
          rhs));
}

inline TableParameters::AnnotationsType &
operator|=(TableParameters::AnnotationsType &lhs,
           TableParameters::AnnotationsType rhs) {
  return lhs = lhs | rhs;
}
} // namespace api
} // namespace engine
} // namespace osram
#endif
