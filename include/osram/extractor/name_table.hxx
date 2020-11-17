#ifndef OSRAM_EXTRACTOR_NAME_TABLE
#define OSRAM_EXTRACTOR_NAME_TABLE

#include <osram/util/indexed_data.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace extractor {
namespace detail {
template <storage::Ownership O> class NameTableImpl;
}

namespace serialization {
template <storage::Ownership O>
inline void read(storage::tar::FileReader &reader, std::string_view name,
                 detail::NameTableImpl<O> &index_data);

template <storage::Ownership O>
inline void write(storage::tar::FileWriter &writer, std::string_view name,
                  const detail::NameTableImpl<O> &index_data);
} // namespace serialization

namespace detail {
template <storage::Ownership O> class NameTableImpl {
public:
  using IndexedData = util::detail::IndexedDataImpl<
      util::serialization::VariableGroupBlock<16, std::string_view>, O>;
  using ResultType = typename IndexedData::ResultType;
  using ValueType = typename IndexedData::ValueType;

private:
  IndexedData indexed_data;

public:
  NameTableImpl() {}

  NameTableImpl(IndexedData indexed_data)
      : indexed_data(std::move(indexed_data)) {}

  std::string_view get_name_for_id(const NameId id) const {
    if (id == INVALID_NAMEID)
      return {};
    return indexed_data.at(id + 0);
  }

  std::string_view get_destinations_for_id(const NameId id) const {
    if (id == INVALID_NAMEID)
      return {};
    return indexed_data.at(id + 1);
  }

  std::string_view get_exits_for_id(const NameId id) const {
    if (id == INVALID_NAMEID)
      return {};
    return indexed_data.at(id + 4);
  }

  std::string_view get_ref_for_id(const NameId id) const {
    if (id == INVALID_NAMEID)
      return {};
    const constexpr auto OFFSET = 3u;
    return indexed_data.at(id + OFFSET);
  }

  std::string_view get_pronunciation_for(const NameId id) const {
    if (id == INVALID_NAMEID)
      return {};
    const constexpr auto OFFSET = 2u;
    return indexed_data.at(id + OFFSET);
  }

  friend void serialization::read(storage::tar::FileReader &reader,
                                  std::string_view name,
                                  detail::NameTableImpl<O> &index_data);
  friend void serialization::write(storage::tar::FileWriter &writer,
                                   std::string_view name,
                                   const detail::NameTableImpl<O> &index_data);
};
} // namespace detail

using NameTable = detail::NameTableImpl<storage::Ownership::Container>;
using NameTableView = detail::NameTableImpl<storage::Ownership::View>;
} // namespace extractor
} // namespace osram
#endif
