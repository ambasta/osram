add_library(${PROJECT_NAME}Utils
    coordinate.cxx
    coordinate_calculation.cxx
    guidance/entry_class.cxx
    guidance/turn_lanes.cxx)

target_include_directories(
  ${PROJECT_NAME}Utils
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
