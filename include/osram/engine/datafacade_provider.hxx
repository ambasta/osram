#ifndef OSRAM_ENGINE_DATAFACADE_PROVIDER
#define OSRAM_ENGINE_DATAFACADE_PROVIDER

#include <memory>

#include <osram/engine/api/base_parameters.hxx>
#include <osram/engine/datafacade.hxx>

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
class ExternalProvider final : public DataFacadeProvider<T, F> {};
} // namespace detail

template <typename T>
using DataFacadeProvider = detail::DataFacadeProvider<T, DataFacade>;

template <typename T>
using ExternalProvider = detail::ExternalProvider<T, DataFacade>;
} // namespace engine
} // namespace osram
#endif
