cmake_minimum_required(VERSION 3.13)
project(phcopy)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

find_package(PkgConfig REQUIRED)
find_package(Gphoto2 REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(LibMTP REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)