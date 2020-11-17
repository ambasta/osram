#ifndef OSRAM_EXTRACTOR_TRAVEL_MODE
#define OSRAM_EXTRACTOR_TRAVEL_MODE

#include <cstdint>
#include <string>

namespace osram {
namespace extractor {

enum TravelMode {
  TRAVEL_MODE_INACCESSIBLE = 0,
  TRAVEL_MODE_DRIVING = 1,
  TRAVEL_MODE_CYCLING = 2,
  TRAVEL_MODE_WALKING = 3,
  TRAVEL_MODE_FERRY = 4,
  TRAVEL_MODE_TRAIN = 5,
  TRAVEL_MODE_PUSHING_BIKE = 6,
  TRAVEL_MODE_STEPS_UP = 8,
  TRAVEL_MODE_STEPS_DOWN = 9,
  TRAVEL_MODE_RIVER_UP = 10,
  TRAVEL_MODE_RIVER_DOWN = 11,
  TRAVEL_MODE_ROUTE = 12,
};

inline std::string travelmode_to_string(const TravelMode mode) {
  std::string token;
  switch (mode) {
  case TRAVEL_MODE_INACCESSIBLE:
    token = "inaccessible";
    break;
  case TRAVEL_MODE_DRIVING:
    token = "driving";
    break;
  case TRAVEL_MODE_CYCLING:
    token = "cycling";
    break;
  case TRAVEL_MODE_WALKING:
    token = "walking";
    break;
  case TRAVEL_MODE_FERRY:
    token = "ferry";
    break;
  case TRAVEL_MODE_TRAIN:
    token = "train";
    break;
  case TRAVEL_MODE_PUSHING_BIKE:
    token = "pushing bike";
    break;
  case TRAVEL_MODE_STEPS_UP:
    token = "steps up";
    break;
  case TRAVEL_MODE_STEPS_DOWN:
    token = "steps down";
    break;
  case TRAVEL_MODE_RIVER_UP:
    token = "river upstream";
    break;
  case TRAVEL_MODE_RIVER_DOWN:
    token = "river downstream";
    break;
  case TRAVEL_MODE_ROUTE:
    token = "route";
    break;
  default:
    token = "other";
    break;
  }
  return token;
}
} // namespace extractor
} // namespace osram
#endif
