add_executable(iphone_photo_download
    app.h
    context.h
    gphoto_camera.h
    gphoto_info.h

    app.cpp
    context.cpp
    gphoto_camera.cpp
    gphoto_info.cpp
    main.cpp)

target_link_libraries(iphone_photo_download PUBLIC
    ${Gphoto2_LIBRARIES}
    ${Boost_LIBRARIES})

target_include_directories(iphone_photo_download PUBLIC
    ${GPHOTO2_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR})
