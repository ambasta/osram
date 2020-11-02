include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE INTERNAL "")

message(CHECK_START "Fetching libosmium")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

fetchcontent_declare(
  libosmium
  GIT_REPOSITORY https://github.com/osmcode/libosmium.git
  GIT_TAG "v2.15.6"
  GIT_SHALLOW TRUE
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libosmium
  PATCH_COMMAND git apply
  "${CMAKE_SOURCE_DIR}/patches/0001-Move-protozero-to-vendored-library.patch"
)
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
set(INSTALL_GDALCPP OFF CACHE INTERNAL "")
set(WITH_PROFILING OFF CACHE INTERNAL "")
set(BUILD_WITH_CCACHE OFF CACHE INTERNAL "")

fetchcontent_makeavailable(libosmium)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Fetched libosmium")

if(NOT libosmium_POPULATED)
  fetchcontent_populate(libosmium)
  add_subdirectory(${libosmium_SOURCE_DIR} ${libosmium_BINARY_DIR})
endif(NOT libosmium_POPULATED)
