#include <gtest/gtest.h>
#include <osrm/engine_config.hpp>
#include <osrm/osrm.hpp>
#include <osrm/table_parameters.hpp>
#include <util/coordinate.hpp>
#include <util/json_container.hpp>

#include <utility>

#include <compass.hxx>

TEST(CompassTest, distanceMatrix) {
  osrm::EngineConfig config;
  config.storage_config = {"data/sample.osrm"};
  config.use_shared_memory = false;
  config.algorithm = osrm::EngineConfig::Algorithm::MLD;

  osrm::OSRM client{config};

  osrm::TableParameters tableParameters;

  std::vector<std::pair<float, float>> coordinates{
      std::make_pair(28.5043, 77.09757), std::make_pair(28.45109, 77.0736)};
  std::vector<std::vector<int64_t>> expected{{0, 703}, {1068, 0}};

  for (auto &coordinate : coordinates) {
    tableParameters.coordinates.emplace_back(
        osrm::util::FloatLongitude{coordinate.second},
        osrm::util::FloatLatitude{coordinate.first});
  }

  std::vector<std::vector<int64_t>> result;

  // Ambience - Sector 44 : 12 mins, 8.1kms, 703s
  // Sector 44 - Ambience : 18 mins, 11.3kms, 1068s
  //
  osrm::engine::api::ResultT response = osrm::json::Object();
  const auto status = client.Table(tableParameters, response);
  ASSERT_TRUE(status == osrm::Status::Ok);

  auto &durations = response.get<osrm::json::Object>()
                        .values["durations"]
                        .get<osrm::json::Array>();

  std::for_each(
      durations.values.begin(), durations.values.end(),
      [&](auto &duration_row) {
        std::vector<int64_t> line_item;
        auto &duration_as_list = duration_row.template get<osrm::json::Array>();

        std::for_each(duration_as_list.values.begin(),
                      duration_as_list.values.end(), [&](auto &value) {
                        line_item.emplace_back(
                            value.template get<osrm::json::Number>().value);
                      });
        result.push_back(line_item);
      });
  ASSERT_EQ(expected, result);
}
