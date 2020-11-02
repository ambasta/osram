#ifndef OSRAM_STORAGE_SHARED_MONITOR
#define OSRAM_STORAGE_SHARED_MONITOR

namespace osram {
    namespace storage {
        template <typename T> struct SharedMonitor {
            using mutex_type = std::shared_mutex;

            SharedMonitor(const T& initial) {
                shmem = shared_memory_object(open_or_create, T::name, read_write);
                std::size_t size = 0;

                if (shmem.get_size(size) && size == 0) {}
            }
        };
    }
}
#endif
