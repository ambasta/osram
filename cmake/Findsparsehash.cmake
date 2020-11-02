# Findlibsparsehash.cmake
#
# Finds the libsparsehash library
#
# This will define the following variables
#
#       libsparsehash_FOUND
#       libsparsehash_INCLUDE_DIRS
#
# and the following targets
#
#       libsparsehash::libsparsehash
#
# Author: amit.ambasta@delhivery.com

find_package(PkgConfig)
pkg_check_modules(PC_sparsehash QUIET libsparsehash)

find_path(
  sparsehash_INCLUDE_DIR
  NAMES sparsetable
  PATHS ${PC_sparsehash_INCLUDE_DIRS}
  PATH_SUFFIXES sparsehash
)
set(sparsehash_VERSION ${PC_sparsehash_VERSION})
mark_as_advanced(sparsehash_FOUND sparsehash_INCLUDE_DIR sparsehash_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  sparsehash
  REQUIRED_VARS sparsehash_INCLUDE_DIR
  VERSION_VAR sparsehash_VERSION
)

if(sparsehash_FOUND)
  set(sparsehash_INCLUDE_DIRS ${libsparsehash_INCLUDE_DIR})
endif(sparsehash_FOUND)

if(sparsehash_FOUND AND NOT TARGET sparsehash::sparsehash)
  add_library(sparsehash::sparsehash INTERFACE IMPORTED)
  set_target_properties(
    sparsehash::sparsehash
    PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${sparsehash_INCLUDE_DIR}
  )
endif(sparsehash_FOUND AND NOT TARGET sparsehash::sparsehash)
