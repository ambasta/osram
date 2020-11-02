# Findgdal.cmake
#
# Finds the gdal library
#
# This will define the following variables
#
#       gdal_FOUND
#       gdal_INCLUDE_DIRS
#       gdal_LIBRARIES
#
# and the following targets
#
#       gdal::gdal
#
# Author: amit.ambasta@delhivery.com

find_package(PkgConfig)
pkg_check_modules(PC_gdal QUIET gdal)

find_path(
  gdal_INCLUDE_DIR
  NAMES gdal.h
  PATHS ${PC_gdal_INCLUDE_DIRS}
  PATH_SUFFIXES gdal
)
set(gdal_VERSION ${PC_gdal_VERSION})
set(gdal_LIBRARIES ${PC_gdal_LIBRARIES})

mark_as_advanced(gdal_FOUND gdal_INCLUDE_DIR gdal_LIBRARIES gdal_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  gdal
  REQUIRED_VARS gdal_INCLUDE_DIR gdal_LIBRARIES
  VERSION_VAR gdal_VERSION
)

if(gdal_FOUND)
  set(gdal_INCLUDE_DIRS ${gdal_INCLUDE_DIR})
  set(gdal_LIBRARIES ${gdal_LIBRARIES})
endif(gdal_FOUND)

if(gdal_FOUND AND NOT TARGET gdal::gdal)
  add_library(gdal::gdal INTERFACE IMPORTED)
  set_target_properties(
    gdal::gdal
    PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${gdal_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES ${gdal_LIBRARIES}
  )
endif(gdal_FOUND AND NOT TARGET gdal::gdal)
