#ifndef OSRAM_ENGINE_DATAFACADE_DATAFACADE_BASE
#define OSRAM_ENGINE_DATAFACADE_DATAFACADE_BASE

#include <osram/engine/approach.hxx>
#include <osram/engine/phantom_node.hxx>
#include <osram/util/coordinate.hxx>

namespace osram {
namespace engine {
namespace datafacade {
class BaseDataFacade {
public:
  BaseDataFacade() {}

  virtual ~BaseDataFacade() {}

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const float, const int,
                                 const int, const Approach,
                                 const bool) const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const float,
                                 const Approach, const bool) const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const unsigned,
                                 const double, const int, const int,
                                 const Approach) const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const unsigned,
                                 const int, const int,
                                 const Approach) const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const unsigned,
                                 const Approach) const = 0;

  virtual std::vector<PhantomNodeWithDistance>
  nearest_phantom_nodes_in_range(const util::Coordinate, const unsigned,
                                 const double, const Approach) const = 0;
};
} // namespace datafacade
} // namespace engine
} // namespace osram
#endif
