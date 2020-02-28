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
#include "gphoto_camera.h"

#include <iostream>

GPhotoCamera::GPhotoCamera(const char* model, const char* port, Context context, const GPhotoInfo& info)
  : context(context) {
    CameraAbilities camera_abilities;
    int ret = GP_OK;
    GPPortInfo port_info;

    ret = gp_camera_new(&camera);
    if (ret < GP_OK) {
        throw std::runtime_error {std::string {"libgphoto2 gp_camera_new failed: "} + gp_result_as_string(ret)};
    }

    if (!info.lookup_camera_ability(model, camera_abilities)) {
        gp_camera_free(camera);
        throw std::runtime_error {"Cannot find camera abilities"};
    }

    ret = gp_camera_set_abilities(camera, camera_abilities);
    if (ret < GP_OK) {
        gp_camera_free(camera);
        throw std::runtime_error {std::string {"libgphoto2 gp_camera_set_abilities failed: "} +
                                  gp_result_as_string(ret)};
    }

    if (!info.lookup_port_path(port, port_info)) {
        gp_camera_free(camera);
        throw std::runtime_error {"Cannot find port information"};
    }

    ret = gp_camera_set_port_info(camera, port_info);
    if (ret < GP_OK) {
        gp_camera_free(camera);
        throw std::runtime_error {std::string {"libgphoto2 gp_camera_set_port_info failed: "} +
                                  gp_result_as_string(ret)};
    }
}

GPhotoCamera::GPhotoCamera(const GPhotoCamera& other) noexcept {
    context = other.context;
    camera = other.camera;
    gp_camera_ref(camera);
}

GPhotoCamera::GPhotoCamera(GPhotoCamera&& other) noexcept {
    context = std::move(other.context);
    camera = other.camera;
    other.camera = nullptr;
}

GPhotoCamera::~GPhotoCamera() {
    if (camera) {
        gp_camera_unref(camera);
    }
}

GPhotoCamera& GPhotoCamera::operator=(const GPhotoCamera& other) noexcept {
    if (this == &other) {
        return *this;
    }

    context = other.context;
    camera = other.camera;
    gp_camera_ref(camera);
    return *this;
}

GPhotoCamera& GPhotoCamera::operator=(GPhotoCamera&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    context = std::move(other.context);
    camera = other.camera;
    other.camera = nullptr;
    return *this;
}

std::vector<std::filesystem::path> GPhotoCamera::list_files(const std::filesystem::path& path) {
    return list_fs(false, path);
}

std::vector<std::filesystem::path> GPhotoCamera::list_folders(const std::filesystem::path& path) {
    return list_fs(true, path);
}

std::vector<std::filesystem::path> GPhotoCamera::list_fs(bool folders, const std::filesystem::path& path) {
    CameraList *list = nullptr;
    gp_list_new(&list);

    int ret = 0;

    if (folders) {
        ret = gp_camera_folder_list_folders(camera, path.c_str(), list, context.get_context());
    } else {
        ret = gp_camera_folder_list_files(camera, path.c_str(), list, context.get_context());
    }

    if (ret < GP_OK) {
        if (folders) {
            std::cerr << "libgphoto2 gp_camera_folder_list_folders failed: ";
        } else {
            std::cerr << "libgphoto2 gp_camera_folder_list_files failed: ";
        }
        std::cerr << gp_result_as_string(ret) << std::endl;
        gp_list_free(list);
        return {};
    }

    int fs_items_count = gp_list_count(list);
    std::vector<std::filesystem::path> result;
    result.reserve(fs_items_count);

    for (int i = 0; i < fs_items_count; i++) {
        const char* item_name;
        gp_list_get_name(list, i, &item_name);
        result.emplace_back(path / item_name);
    }

    gp_list_free(list);

    return result;
}
