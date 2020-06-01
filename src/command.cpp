// phcopy
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
#include "command.h"

#include <iostream>

Command::Command() : info(context) {}

void Command::execute() {}

void Command::load_camera_info() {
    if (!info.load_cameras_abilities()) {
        throw std::runtime_error("Failed to load camera capabilities");
    }

    if (!info.load_port_info()) {
        throw std::runtime_error("Failed to load information about ports");
    }
}

Context& Command::get_context() {
    return context;
}
GPhotoInfo& Command::get_gphoto_info() {
    return info;
}

CameraList* Command::autodetect_cameras() const {
    CameraList* list = nullptr;
    gp_list_new(&list);

    int ret = gp_camera_autodetect(list, context.get_context());
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 autodetect failed: " << gp_result_as_string(ret) << std::endl;
        return nullptr;
    }

    return list;
}

GPhotoCamera Command::open_camera(size_t idx) {
    CameraList* list = autodetect_cameras();

    if (list == nullptr) {
        throw std::runtime_error {"No cameras available"};
    }

    int devices_num = gp_list_count(list);

    if (idx >= devices_num) {
        gp_list_free(list);
        throw std::runtime_error {"Device index is out of range"};
    }

    const char *name, *port;
    gp_list_get_name(list, idx, &name);
    gp_list_get_value(list, idx, &port);

    try {
        GPhotoCamera camera(name, port, context, info);
        gp_list_free(list);

        return camera;
    } catch (std::runtime_error& e) {
        gp_list_free(list);
        throw;
    }
}
