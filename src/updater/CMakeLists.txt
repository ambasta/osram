add_library(
  ${PROJECT_NAME}Updater
  updater.cxx
)

target_include_directories(
  ${PROJECT_NAME}Updater
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
