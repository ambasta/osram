# Findgeos.cmake
#
# Finds the geos library
#
# This will define the following variables
#
#       geos_FOUND
#       geos_INCLUDE_DIRS
#       geos_LIBRARIES
#
# and the following targets
#
#       geos::geos
#
# Author: amit.ambasta@delhivery.com

find_path(geos_INCLUDE_DIR geos/geom.h)
find_library(geos_LIBRARY NAMES geos)

set(geos_VERSION "unknown")

mark_as_advanced(geos_FOUND geos_INCLUDE_DIR geos_LIBRARY geos_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(geos REQUIRED_VARS geos_INCLUDE_DIR geos_LIBRARY VERSION_VAR geos_VERSION)

if(geos_FOUND)
  set(geos_INCLUDE_DIRS ${geos_INCLUDE_DIR})
  set(geos_LIBRARIES ${geos_LIBRARY})
endif(geos_FOUND)

if(geos_FOUND AND NOT TARGET geos::geos)
  add_library(geos::geos INTERFACE IMPORTED)
  set_target_properties(
    geos::geos
    PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${geos_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${geos_LIBRARY}"
  )
endif(geos_FOUND AND NOT TARGET geos::geos)
