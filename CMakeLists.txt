cmake_minimum_required(VERSION 3.15)
project(iphone_photo_download)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)

pkg_check_modules(USB REQUIRED IMPORTED_TARGET libusb-1.0)

add_executable(iphone_photo_download main.cpp)

target_link_libraries(iphone_photo_download PUBLIC ${USB_LIBRARIES})
target_include_directories(iphone_photo_download PUBLIC ${USB_INCLUDE_DIRS})