cmake_minimum_required(VERSION 3.13)
project(phcopy)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

option(BUILD_TESTS "Build unit tests" OFF)

find_package(PkgConfig REQUIRED)
find_package(Gphoto2 REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)

if(BUILD_TESTS)
  enable_testing()
  set(INSTALL_GTEST OFF CACHE BOOL "Disable installing GTest" FORCE)
  add_subdirectory(lib/googletest)
  add_subdirectory(test)
endif()