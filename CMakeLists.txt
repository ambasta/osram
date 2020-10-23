cmake_minimum_required(VERSION 3.10)

PROJECT(osram LANGUAGES CXX)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
Set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} src/main.cxx)
target_include_directories(${PROJECT_NAME} PUBLIC include/)
