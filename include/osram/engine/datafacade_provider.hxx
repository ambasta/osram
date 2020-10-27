#ifndef OSRAM_ENGINE_DATAFACADE_PROVIDER
#define OSRAM_ENGINE_DATAFACADE_PROVIDER

#include <memory>

#include <osram/engine/api/base_parameters.hxx>
#include <osram/engine/datafacade.hxx>
#include <osram/engine/datafacade/mmap_memory_allocator.hxx>
#include <osram/engine/datafacade/process_memory_allocator.hxx>
#include <osram/engine/datafacade_factory.hxx>

namespace osram {
namespace engine {
namespace detail {
template <typename T, template <typename A> class F> class DataFacadeProvider {
public:
  using Facade = F<T>;

  virtual ~DataFacadeProvider() = default;

  virtual std::shared_ptr<const Facade>
  get(const api::BaseParameters &) const = 0;
};

template <typename T, template <typename A> class F>
class ExternalProvider final : public DataFacadeProvider<T, F> {
private:
  DataFacadeFactory<F, T> facade_factory;

public:
  using Facade = typename DataFacadeProvider<T, F>::Facade;

  ExternalProvider(const storage::StorageConfig &config)
      : facade_factory(
            std::make_shared<datafacade::MMapMemoryAllocator>(config)) {}

  /*
   * We avoid tileparameters for now
   * std::shared_ptr<const Facade> get(const api::TileParameters &params) const
  override final { return facade_factory.get(params);
  } */

  std::shared_ptr<const Facade>
  get(const api::BaseParameters &params) const override final {
    return facade_factory.get(params);
  }
};

template <typename T, template <typename A> class F>
class ImmutableProvider final : public DataFacadeProvider<T, F> {
private:
  DataFacadeFactory<F, T> facade_factory;

public:
  using Facade = typename DataFacadeProvider<T, F>::Facade;

  ImmutableProvider(const storage::StorageConfig &config)
      : facade_factory(
            std::make_shared<datafacade::ProcessMemoryAllocator>(config)) {}

  std::shared_ptr<const Facade>
  get(const api::BaseParameters &params) const override final {
    return facade_factory.get(params);
  }
};
} // namespace detail

template <typename T>
using DataFacadeProvider = detail::DataFacadeProvider<T, DataFacade>;
template <typename T>
using ImmutableProvider = detail::ImmutableProvider<T, DataFacade>;
template <typename T>
using ExternalProvider = detail::ExternalProvider<T, DataFacade>;
} // namespace engine
} // namespace osram
#endif
