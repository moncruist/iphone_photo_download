add_executable(phcopy
    app.h
    context.h
    gphoto_camera.h
    gphoto_info.h
    folder_pair.h

    app.cpp
    context.cpp
    gphoto_camera.cpp
    gphoto_info.cpp
    main.cpp)

target_link_libraries(phcopy PUBLIC
    ${Gphoto2_LIBRARIES}
    ${Boost_LIBRARIES})

target_include_directories(phcopy PUBLIC
    ${GPHOTO2_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR})
