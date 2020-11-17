#ifndef OSRAM_UTIL_WORKING_HOURS
#define OSRAM_UTIL_WORKING_HOURS

#include <chrono>
#include <string>
#include <vector>

namespace osram {
namespace util {
struct WorkingHours {
  enum Modifier {
    unknown,
    open,
    closed,
    off,
    is24_7,
  };

  Modifier modifer;

  WorkingHours() : modifer(open) {}

  bool is_in_range(const std::chrono::sys_time<std::chrono::seconds>) const;
};

std::vector<WorkingHours> parse_working_hours(std::string_view);

bool check_working_hours(std::vector<WorkingHours> &,
                         const std::chrono::sys_time<std::chrono::seconds>);
} // namespace util
} // namespace osram
#endif
