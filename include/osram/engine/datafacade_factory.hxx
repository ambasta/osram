#ifndef OSRAM_ENGINE_DATAFACADE_FACTORY
#define OSRAM_ENGINE_DATAFACADE_FACTORY

#include <algorithm>
#include <osram/engine/algorithm.hxx>
#include <osram/engine/api/base_parameters.hxx>
#include <osram/extractor/class_data.hxx>
#include <osram/extractor/profile_properties.hxx>
#include <range/v3/view/iota.hpp>
#include <unordered_map>
#include <vector>

namespace osram {
namespace engine {
template <template <typename A> class F, typename T> class DataFacadeFactory {
public:
  using Facade = F<T>;

private:
  static constexpr auto has_exclude_flags = algorithm::has_exclude_flags<T>{};
  std::vector<std::shared_ptr<const Facade>> facades;
  std::unordered_map<std::string_view, extractor::ClassData> name_to_class;
  const extractor::ProfileProperties *properties = nullptr;

  template <typename AllocatorT>
  DataFacadeFactory(std::shared_ptr<AllocatorT> allocator, std::true_type) {
    const auto &index = allocator->get_index();
    properties = index.template get_block_ptr<extractor::ProfileProperties>(
        "/common/properties");
    const auto &metric_name = properties->get_weight_name();

    std::vector<std::string> exclude_prefixes;
    auto exclude_path = std::string{"/"} + algorithm::identifier<T>() +
                        std::string{"/metrics/"} + metric_name + "/exclude/";
    index.list(exclude_path, std::back_inserter(exclude_prefixes));
    facades.resize(exclude_prefixes.size());

    if (facades.empty())
      throw std::runtime_error("Could not find any metrics for " +
                               algorithm::name<T>() +
                               " in the data. Did you load the right dataset?");

    for (const auto &exclude_prefix : exclude_prefixes) {
      auto index_begin = exclude_prefix.find_last_of("/");
      assert(("The exclude prefix needs to be a valid data path.",
              index_begin != std::string::npos));
      std::size_t index = std::stoi(
          exclude_prefix.substr(index_begin + 1, exclude_prefix.size()));
      assert(index >= 0 and index < facades.size());
      facades[index] =
          std::make_shared<const Facade>(allocator, metric_name, index);
    }

    for (const auto index :
         ranges::views::ints(properties->class_names.size())) {
      std::string_view name = properties->get_class_name(index);
      if (!name.empty()) {
        name_to_class[name] = extractor::getClassData(index);
      }
    }
  }

  template <typename AllocatorT>
  DataFacadeFactory(std::shared_ptr<AllocatorT> allocator, std::false_type) {
    const auto &index = allocator->get_index();
    properties = index.template get_block_ptr<extractor::ProfileProperties>(
        "/common/properties");
    const auto &metric_name = properties->get_weight_name();
    facades.push_back(
        std::make_shared<const Facade>(allocator, metric_name, 0));
  }

  std::shared_ptr<const Facade> get(const api::BaseParameters &params,
                                    std::false_type) const {
    if (!params.exclude.empty())
      return {};
    return facades[0];
  }

  std::shared_ptr<const Facade> get(const api::BaseParameters &params,
                                    std::true_type) const {
    if (params.exclude.empty())
      return facades[0];

    extractor::ClassData mask = 0;
    for (const auto &name : params.exclude) {
      auto class_mask_iter = name_to_class.find(name);

      if (class_mask_iter == name_to_class.end())
        return {};

      mask |= class_mask_iter->second;
    }

    auto exclude_iter = std::find(properties->excludable_classes.begin(),
                                  properties->excludable_classes.end(), mask);

    if (exclude_iter == properties->excludable_classes.end())
      return {};

    auto exclude_index =
        std::distance(properties->excludable_classes.begin(), exclude_iter);
    return facades[exclude_index];
  }

public:
  DataFacadeFactory() = default;

  template <typename AllocatorT>
  DataFacadeFactory(std::shared_ptr<AllocatorT> allocator)
      : DataFacadeFactory(allocator, has_exclude_flags) {
    assert(("At least one datafacade is needed", facades.size() >= 1));
  }
  template <typename ParameterT>
  std::shared_ptr<const Facade> get(const ParameterT &params) const {
    return get(params, has_exclude_flags);
  }
};
} // namespace engine
} // namespace osram
#endif
