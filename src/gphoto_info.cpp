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

GPhotoInfo::GPhotoInfo(Context context) noexcept
  : context(context), port_info_list(nullptr, gp_port_info_list_free), abilities(nullptr, gp_abilities_list_free) {}

GPhotoInfo::~GPhotoInfo() {}

bool GPhotoInfo::load_port_info() noexcept {
    if (port_info_list) {
        // Don't load already loaded list
        return true;
    }

    GPPortInfoList* list = nullptr;
    int ret = 0;
    /* Load all the port drivers we have... */
    ret = gp_port_info_list_new(&list);

    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_port_info_list_new failed: " << gp_result_as_string(ret) << std::endl;
        return false;
    }

    port_info_list.reset(list);
    list = nullptr;

    ret = gp_port_info_list_load(port_info_list.get());
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_port_info_list_load failed: " << gp_result_as_string(ret) << std::endl;
        port_info_list.reset();
        return false;
    }

    ret = gp_port_info_list_count(port_info_list.get());
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_port_info_list_count failed: " << gp_result_as_string(ret) << std::endl;
        port_info_list.reset();
        return false;
    }
    return true;
}

bool GPhotoInfo::load_cameras_abilities() noexcept {
    int ret = 0;

    if (abilities != nullptr) {
        // Don't load already loaded list
        return true;
    }

    CameraAbilitiesList* list = nullptr;
    /* Load all the camera drivers we have... */
    ret = gp_abilities_list_new(&list);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_abilities_list_new failed: " << gp_result_as_string(ret) << std::endl;
        return false;
    }

    abilities.reset(list);
    list = nullptr;

    ret = gp_abilities_list_load(abilities.get(), context.get_context());
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_abilities_list_load failed: " << gp_result_as_string(ret) << std::endl;
        abilities.reset();
        return false;
    }

    return true;
}

bool GPhotoInfo::lookup_camera_ability(const char* model, CameraAbilities& abilities_out) const noexcept {
    if (abilities == nullptr) {
        return false;
    }

    int m = 0, ret = 0;

    /* First lookup the model / driver */
    m = gp_abilities_list_lookup_model(abilities.get(), model);
    if (m < GP_OK) {
        std::cerr << "libgphoto2 gp_abilities_list_lookup_model failed: " << gp_result_as_string(ret) << std::endl;
        return false;
    }

    ret = gp_abilities_list_get_abilities(abilities.get(), m, &abilities_out);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_abilities_list_get_abilities failed: " << gp_result_as_string(ret) << std::endl;
        return false;
    }

    return true;
}

bool GPhotoInfo::lookup_port_path(const char* port, GPPortInfo& port_info) const noexcept {
    if (!port_info_list) {
        return false;
    }

    /* Then associate the camera with the specified port */
    int port_idx = gp_port_info_list_lookup_path(port_info_list.get(), port);
    if (port_idx < GP_OK) {
        if (port_idx == GP_ERROR_UNKNOWN_PORT) {
            std::cerr << "Cannot find port: " << port << std::endl;
        } else {
            std::cerr << "libgphoto2 gp_port_info_list_lookup_path failed: " << gp_result_as_string(port_idx)
                      << std::endl;
        }
        return false;
    }

    int ret = gp_port_info_list_get_info(port_info_list.get(), port_idx, &port_info);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_port_info_list_get_info failed: " << gp_result_as_string(ret) << std::endl;
        return false;
    }

    return true;
}
