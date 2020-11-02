cmake_minimum_required(VERSION 3.10)

project(osram LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(vendor)
add_subdirectory(src)

# # Add Tests
# enable_testing()
# add_subdirectory(tests)
