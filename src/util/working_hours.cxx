#include <chrono>
#include <osram/util/working_hours.hxx>

namespace osram {
namespace util {
bool WorkingHours::is_in_range(
    const std::chrono::sys_time<std::chrono::seconds> sys_time) const {
  bool use_current_day = true, use_next_day = false;

  // TODO
  return use_current_day or use_next_day;
}

std::vector<WorkingHours> parse_working_hours(std::string_view input) {
  return std::vector<WorkingHours>{};
}

bool check_working_hours(
    std::vector<WorkingHours> &working_hours,
    const std::chrono::sys_time<std::chrono::seconds> &time) {
  bool is_open = false;

  for (auto &working_hour : working_hours) {
    if (working_hour.modifer == WorkingHours::is24_7) {
      return true;
    }

    if (working_hour.is_in_range(time)) {
      is_open = working_hour.modifer == WorkingHours::open;
    }
  }
  return is_open;
}
} // namespace util
} // namespace osram
