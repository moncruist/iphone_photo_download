cmake_minimum_required(VERSION 3.15)
project(iphone_photo_download)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

find_package(PkgConfig REQUIRED)
find_package(Gphoto2 REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(iphone_photo_download
    app.h
    context.h

    app.cpp
    context.cpp
    main.cpp)

target_link_libraries(iphone_photo_download PUBLIC
    ${Gphoto2_LIBRARIES}
    ${Boost_LIBRARIES})

target_include_directories(iphone_photo_download PUBLIC
    ${GPHOTO2_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR})
