find_package(GTest QUIET)

if(NOT GTEST_FOUND)
  configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/GoogleTest-CMakeLists.txt.in
    ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt
  )
  execute_process(
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
    RESULT_VARIABLE result WORKING_DIRECTORY
    ${CMAKE_BINARY_DIR}/googletest-download
  )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif(result)

  execute_process(
    COMMAND ${CMAKE_COMMAND} --build . RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
  )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif(result)

  add_subdirectory(
      ${CMAKE_SOURCE_DIR}/googletest-src
    ${CMAKE_BINARY_DIR}/googletest-build EXCLUDE_FROM_ALL
  )
endif(NOT GTEST_FOUND)

add_executable(test_route src/test_route.cxx)
target_compile_features(test_route PRIVATE cxx_auto_type)
target_link_libraries(test_route gtest_main ${PROJECT_NAME})

message("crod ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

add_test(
  NAME setup_fixtures COMMAND cmake -E tar xvf
  ${CMAKE_CURRENT_SOURCE_DIR}/sample.tar.bz2
)
add_test(NAME test_route COMMAND test_route)
add_test(NAME clean_fixtures COMMAND cmake -E rm -r ${CMAKE_CURRENT_BINARY_DIR}/data)

set_tests_properties(setup_fixtures PROPERTIES FIXTURES_SETUP osrm_fixtures)
set_tests_properties(test_route PROPERTIES FIXTURES_REQUIRED osrm_fixtures)
set_tests_properties(clean_fixtures PROPERTIES FIXTURES_CLEANUP osrm_fixtures)
