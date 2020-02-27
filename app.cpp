#include "app.h"

#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2.h>
#include <iostream>


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
    Camera *camera = nullptr;
    CameraAbilitiesList* abilities = nullptr;
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

    /* Load all the camera drivers we have... */
    ret = gp_abilities_list_new(&abilities);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_abilities_list_new failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    ret = gp_abilities_list_load(abilities, context.get_context());
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_abilities_list_load failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    /* First lookup the model / driver */
    m = gp_abilities_list_lookup_model(abilities, name);
    if (m < GP_OK) {
        std::cerr << "libgphoto2 gp_abilities_list_lookup_model failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    ret = gp_abilities_list_get_abilities(abilities, m, &camera_abilities);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_abilities_list_get_abilities failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    ret = gp_camera_set_abilities(camera, camera_abilities);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_camera_set_abilities failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    /* Load all the port drivers we have... */
    ret = gp_port_info_list_new(&port_info_list);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_port_info_list_new failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    ret = gp_port_info_list_load(port_info_list);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_port_info_list_load failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    ret = gp_port_info_list_count(port_info_list);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_port_info_list_count failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    /* Then associate the camera with the specified port */
    port_idx = gp_port_info_list_lookup_path(port_info_list, port);
    if (port_idx < GP_OK) {
        if (port_idx == GP_ERROR_UNKNOWN_PORT) {
            std::cerr << "Cannot find port: " << port << std::endl;
        } else {
            std::cerr << "libgphoto2 gp_port_info_list_lookup_path failed: " << gp_result_as_string(port_idx)
                      << std::endl;
        }
        goto fail;
    }

    ret = gp_port_info_list_get_info(port_info_list, port_idx, &port_info);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_port_info_list_get_info failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    ret = gp_camera_set_port_info(camera, port_info);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_camera_set_port_info failed: " << gp_result_as_string(ret) << std::endl;
        goto fail;
    }

    gp_port_info_list_free(port_info_list);
    gp_abilities_list_free(abilities);

    return camera;

fail:
    if (port_info_list != nullptr) {
        gp_port_info_list_free(port_info_list);
    }

    if (abilities != nullptr) {
        gp_abilities_list_free(abilities);
    }
    if (camera != nullptr) {
        gp_camera_free(camera);
    }
    return nullptr;
}
