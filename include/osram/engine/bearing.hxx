#ifndef OSRAM_ENGINE_BEARING
#define OSRAM_ENGINE_BEARING

namespace osram {
    namespace engine {
        struct Bearing {
            short bearing;
            short range;

            bool is_valid() const {
                return bearing >= 0 and bearing <= 360 and range >= 0 and range <= 180;
            }
        };

        inline bool operator==(const Bearing lhs, const Bearing rhs) {
            return lhs.bearing == rhs.bearing and lhs.range == rhs.range;
        }
    }
}
#endif
