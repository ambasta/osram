add_subdirectory(extractor)
add_subdirectory(util)
# add_subdirectory(updater)
add_subdirectory(storage)
add_subdirectory(engine)

add_library(${PROJECT_NAME} osram.cxx)

target_link_libraries(
  ${PROJECT_NAME}
  ${PROJECT_NAME}Storage
  ${PROJECT_NAME}Util
  ${PROJECT_NAME}Engine
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
