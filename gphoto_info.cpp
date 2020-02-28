// iPhone photo download
// Copyright (C) 2020 Konstantin Zhukov
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
#include "gphoto_info.h"

#include <iostream>

GPhotoInfo::GPhotoInfo(Context context) noexcept : context(context) {}

GPhotoInfo::~GPhotoInfo() {
    if (port_info_list) {
        gp_port_info_list_free(port_info_list);
    }
    if (abilities) {
        gp_abilities_list_free(abilities);
    }
}

bool GPhotoInfo::load_port_info() noexcept {
    if (port_info_list != nullptr) {
        // Don't load already loaded list
        return true;
    }

    int ret = 0;
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
    return true;

fail:
    if (port_info_list != nullptr) {
        gp_port_info_list_free(port_info_list);
        port_info_list = nullptr;
    }
    return false;
}

bool GPhotoInfo::load_cameras_abilities() noexcept {
    int ret = 0;

    if (abilities != nullptr) {
        // Don't load already loaded list
        return true;
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

    return true;

fail:
    if (abilities != nullptr) {
        gp_abilities_list_free(abilities);
        abilities = nullptr;
    }
    return false;
}

bool GPhotoInfo::lookup_camera_ability(const char* model, CameraAbilities& abilities_out) const noexcept {
    if (abilities == nullptr) {
        return false;
    }

    int m = 0, ret = 0;

    /* First lookup the model / driver */
    m = gp_abilities_list_lookup_model(abilities, model);
    if (m < GP_OK) {
        std::cerr << "libgphoto2 gp_abilities_list_lookup_model failed: " << gp_result_as_string(ret) << std::endl;
        return false;
    }

    ret = gp_abilities_list_get_abilities(abilities, m, &abilities_out);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_abilities_list_get_abilities failed: " << gp_result_as_string(ret) << std::endl;
        return false;
    }

    return true;
}

bool GPhotoInfo::lookup_port_path(const char* port, GPPortInfo& port_info) const noexcept {
    if (port_info_list == nullptr) {
        return false;
    }

    /* Then associate the camera with the specified port */
    int port_idx = gp_port_info_list_lookup_path(port_info_list, port);
    if (port_idx < GP_OK) {
        if (port_idx == GP_ERROR_UNKNOWN_PORT) {
            std::cerr << "Cannot find port: " << port << std::endl;
        } else {
            std::cerr << "libgphoto2 gp_port_info_list_lookup_path failed: " << gp_result_as_string(port_idx)
                      << std::endl;
        }
        return false;
    }

    int ret = gp_port_info_list_get_info(port_info_list, port_idx, &port_info);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_port_info_list_get_info failed: " << gp_result_as_string(ret) << std::endl;
        return false;
    }

    return true;
}
