#include "app.h"

#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2.h>
#include <iostream>

#include "gphoto_camera.h"

App::App() : info(context) {
    if (!info.load_cameras_abilities()) {
        throw std::runtime_error("Failed to load camera capabilities");
    }

    if (!info.load_port_info()) {
        throw std::runtime_error("Failed to load information about ports");
    }
}

void App::print_device_list() {
    CameraList* list = autodetect_cameras();

    if (list == nullptr) {
        return;
    }

    int devices_num = gp_list_count(list);

    std::cout << "Found devices: " << devices_num << std::endl;

    for (int i = 0; i < devices_num; i++) {
        const char *name, *port;
        gp_list_get_name(list, i, &name);
        gp_list_get_value(list, i, &port);

        std::cout << "[" << i << "]: " << name << " / " << port << std::endl;
        std::cout << std::endl;
    }
    gp_list_free(list);
}


void App::list_files(size_t idx, const std::string& path) {
    CameraList* list = autodetect_cameras();

    if (list == nullptr) {
        return;
    }

    int devices_num = gp_list_count(list);

    if (idx >= devices_num) {
        std::cerr << "Idx out of range" << std::endl;
        gp_list_free(list);
        return;
    }

    const char *name, *port;
    gp_list_get_name(list, idx, &name);
    gp_list_get_value(list, idx, &port);

    try {
        GPhotoCamera camera(name, port, context, info);
        auto folders = camera.list_folders(path);
        auto files = camera.list_files(path);

        for (const auto& folder : folders) {
            std::cout << path << folder << "/" << std::endl;
        }
        for (const auto& file : files) {
            std::cout << path << file << std::endl;
        }
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
    gp_list_free(list);
}

CameraList* App::autodetect_cameras() const {
    CameraList* list = nullptr;
    gp_list_new(&list);

    int ret = gp_camera_autodetect(list, context.get_context());
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 autodetect failed: " << gp_result_as_string(ret) << std::endl;
        return nullptr;
    }

    return list;
}
