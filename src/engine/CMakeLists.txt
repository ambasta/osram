add_library(
  ${PROJECT_NAME}Engine
  engine_config.cxx
  hint.cxx
  search_engine_data.cxx
)

target_include_directories(
  ${PROJECT_NAME}Engine
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
