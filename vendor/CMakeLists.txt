find_package(osmium QUIET)

if(NOT osmium_FOUND)
  add_subdirectory(osmium)
endif(NOT osmium_FOUND)
