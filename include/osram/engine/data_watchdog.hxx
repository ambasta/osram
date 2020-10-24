#ifndef OSRAM_ENGINE_DATA_WATCHDOG
#define OSRAM_ENGINE_DATA_WATCHDOG
#include "osram/engine/datafacade/contiguous_internalmem_datafacade.hxx"
#include "osram/engine/datafacade_factory.hxx"
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string_view>

namespace osram {
namespace engine {
namespace detail {
template <typename T, typename F> class DataWatchDogImpl;

template <typename T>
class DataWatchDogImpl<T, datafacade::ContiguousInternalMemoryDataFacade<T>>
    final {
  using mutex_type = typename storage::SharedMonitor<
      storage::SharedRegionRegister>::mutex_type;
  using Facade = datafacade::ContiguousInternalMemoryDataFacade<T>;

private:
  mutable std::shared_mutex factory_mutex;
  const std::string dataset_name;
  storage::SharedMonitor<storage::SharedRegionRegister> barrier;
  std::thread watcher;
  bool active;
  storage::SharedRegion static_region, updateable_region, *static_shared_region,
      *updateable_shared_region;
  DataFacadeFactory<datafacade::ContiguousInternalMemoryDataFacade, T>
      facade_factory;

  void run() {
    while (active) {
      std::scoped_lock<mutex_type> current_region_lock(barrier.get_mutex());

      while (active and
             static_region.timestamp == static_shared_region->timestamp and
             updateable_region.timestamp == updateable_shared_region->timestamp)
        barrier.wait(current_region_lock);

      if (!active)
        break;

      if (static_region.timestamp != static_shared_region->timestamp)
        static_region = *static_shared_region;

      if (updateable_region.timestamp != updateable_shared_region->timestamp)
        updateable_region = *updateable_shared_region;

      {
        std::unique_lock<std::shared_mutex> swap_lock(factory_mutex);
        facade_factory =
            DataFacadeFactory<datafacade::ContiguousInternalMemoryDataFacade,
                              T>(
                std::make_shared<datafacade::SharedMemoryAllocator>(
                    std::vector<storage::SharedRegionRegister::ShmKey>{
                        static_region.shm_key, updateable_region.shm_key}));
      }
    }
  }

public:
  DataWatchDogImpl(std::string_view dataset_name)
      : dataset_name(dataset_name), active(true) {
    {
      std::scoped_lock<mutex_type> current_region_lock(barrier.get_mutex());
      auto &shared_register = barrier.data();
      auto static_region_id = shared_register.find(dataset_name + "/static");
      auto updateable_region_id =
          shared_register.find(dataset_name + "/updateable");

      if (static_region_id ==
              storage::SharedRegionRegister::INVALID_REGION_ID or
          updateable_region_id ==
              storage::SharedRegionRegister::INVALID_REGION_ID)
        throw std::runtime_error("Could not find shared memory region for \"" +
                                 dataset_name +
                                 "/data\". Did you run osrm-datastore?");
      static_shared_region = &shared_register.get_region(static_region_id);
      updateable_shared_region =
          &shared_register.get_region(updateable_region_id);
      static_region = *static_shared_region;
      updateable_region = *updateable_shared_region;

      {
        std::unique_lock<std::shared_mutex> swap_lock(factory_mutex);
        facade_factory =
            DataFacadeFactory<datafacade::ContiguousInternalMemoryDataFacade,
                              T>(
                std::make_shared<datafacade::SharedMemoryAllocator>(
                    std::vector<storage::SharedRegionRegister::ShmKey>{
                        static_region.shm_key, updateable_region.shm_key}));
      }
    }
    watcher = std::thread(&DataWatchDogImpl::run, this);
  }

  ~DataWatchDogImpl() {
    active = false;
    barrier.notify_all();
    watcher.join();
  }

  std::shared_ptr<const Facade> get(const api::BaseParameters &params) const {
    std::shared_lock<std::shared_mutex> swap_lock(factory_mutex);
    return facade_factory.get(params);
  }
};
} // namespace detail

template <typename T, template <typename A> class F>
using DataWatchDog = detail::DataWatchDogImpl<T, F<T>>;
} // namespace engine
} // namespace osram
#endif
