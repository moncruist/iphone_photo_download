#include "app.h"

#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2.h>
#include <iostream>

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


void App::open_camera(size_t idx) {
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

    Camera* camera = open_camera(name, port);
    gp_list_free(list);
    gp_camera_free(camera);
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

Camera* App::open_camera(const char* name, const char* port) const {
    Camera* camera = nullptr;

    GPPortInfoList* port_info_list = nullptr;
    CameraAbilities camera_abilities;
    int ret = GP_OK;
    int m = 0;
    int port_idx = 0;
    GPPortInfo port_info;

    ret = gp_camera_new(&camera);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_camera_new failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    if (!info.lookup_camera_ability(name, camera_abilities)) {
        goto fail;
    }

    ret = gp_camera_set_abilities(camera, camera_abilities);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_camera_set_abilities failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    if (!info.lookup_port_path(port, port_info)) {
        goto fail;
    }

    ret = gp_camera_set_port_info(camera, port_info);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_camera_set_port_info failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    gp_port_info_list_free(port_info_list);

    return camera;

fail:

    if (camera != nullptr) {
        gp_camera_free(camera);
    }
    return nullptr;
}
