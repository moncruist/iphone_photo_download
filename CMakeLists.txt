cmake_minimum_required(VERSION 3.13)
project(iphone_photo_download)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

find_package(PkgConfig REQUIRED)
find_package(Gphoto2 REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

add_subdirectory(src)