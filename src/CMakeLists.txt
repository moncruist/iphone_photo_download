add_executable(phcopy
    context.h
    command.h
    download_command.h
    gphoto_camera.h
    gphoto_info.h
    folder_pair.h
    list_devices_command.h
    list_files_command.h

    context.cpp
    command.cpp
    download_command.cpp
    gphoto_camera.cpp
    gphoto_info.cpp
    main.cpp
    list_devices_command.cpp
    list_files_command.cpp)

target_link_libraries(phcopy PUBLIC
    ${Gphoto2_LIBRARIES}
    ${Boost_LIBRARIES})

target_include_directories(phcopy PUBLIC
    ${GPHOTO2_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR})
