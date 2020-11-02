find_package(osmium REQUIRED)

add_library(${PROJECT_NAME}Extractor extractor.cxx)

target_link_libraries(${PROJECT_NAME}Extractor osmium::osmium)

target_include_directories(
  ${PROJECT_NAME}Extractor PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
