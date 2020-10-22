cmake_minimum_required(VERSION 3.10)

PROJECT(osram LANGUAGES CXX)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
Set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include/)

add_executable(${PROJECT_NAME}main src/main.cxx)
target_link_libraries(${PROJECT_NAME}main ${PROJECT_NAME})
