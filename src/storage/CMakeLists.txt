add_library(
  ${PROJECT_NAME}Storage
  io_config.cxx
)

target_include_directories(
  ${PROJECT_NAME}Storage
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
