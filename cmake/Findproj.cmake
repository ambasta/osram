# Findproj.cmake
#
# Finds the proj library
#
# This will define the following variables
#
#       proj_FOUND
#       proj_INCLUDE_DIRS
#       proj_LIBRARIES
#
# and the following targets
#
#       proj::proj
#
# Author: amit.ambasta@delhivery.com

find_package(PkgConfig)
pkg_check_modules(PC_proj QUIET proj)

find_path(
  proj_INCLUDE_DIR
  NAMES proj.h
  PATHS ${PC_proj_INCLUDE_DIRS}
  PATH_SUFFIXES proj
)
set(proj_VERSION ${PC_proj_VERSION})
set(proj_LIBRARIES ${PC_proj_LIBRARIES})

mark_as_advanced(proj_FOUND proj_INCLUDE_DIR proj_LIBRARIES proj_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  proj
  REQUIRED_VARS proj_INCLUDE_DIR proj_LIBRARIES
  VERSION_VAR proj_VERSION
)

if(proj_FOUND)
  set(proj_INCLUDE_DIRS ${proj_INCLUDE_DIR})
  set(proj_LIBRARIES ${proj_LIBRARIES})
endif(proj_FOUND)

if(proj_FOUND AND NOT TARGET proj::proj)
  add_library(proj::proj INTERFACE IMPORTED)
  set_target_properties(
    proj::proj
    PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${proj_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES ${proj_LIBRARIES}
  )
endif(proj_FOUND AND NOT TARGET proj::proj)
