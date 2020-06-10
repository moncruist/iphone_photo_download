add_library(phcopy_logic STATIC
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
  list_devices_command.cpp
  list_files_command.cpp)

target_link_libraries(phcopy_logic PUBLIC
  ${Gphoto2_LIBRARIES})


add_executable(phcopy
    main.cpp)

target_link_libraries(phcopy PUBLIC
    phcopy_logic
    ${Boost_LIBRARIES})

target_include_directories(phcopy PUBLIC
    ${Boost_INCLUDE_DIR})
