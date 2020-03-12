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
#include "gphoto_camera.h"

#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

GPhotoCamera::GPhotoCamera(const char* model, const char* port, Context context, const GPhotoInfo& info)
  : context(context), camera(nullptr) {
    CameraAbilities camera_abilities;
    int ret = GP_OK;
    GPPortInfo port_info;

    {
        Camera* ptr = nullptr;
        ret = gp_camera_new(&ptr);
        if (ret < GP_OK) {
            throw std::runtime_error {std::string {"libgphoto2 gp_camera_new failed: "} + gp_result_as_string(ret)};
        }
        camera = std::shared_ptr<Camera>(ptr, gp_camera_free);
    }

    if (!info.lookup_camera_ability(model, camera_abilities)) {
        throw std::runtime_error {"Cannot find camera abilities"};
    }

    ret = gp_camera_set_abilities(camera.get(), camera_abilities);
    if (ret < GP_OK) {
        throw std::runtime_error {std::string {"libgphoto2 gp_camera_set_abilities failed: "} +
                                  gp_result_as_string(ret)};
    }

    if (!info.lookup_port_path(port, port_info)) {
        throw std::runtime_error {"Cannot find port information"};
    }

    ret = gp_camera_set_port_info(camera.get(), port_info);
    if (ret < GP_OK) {
        throw std::runtime_error {std::string {"libgphoto2 gp_camera_set_port_info failed: "} +
                                  gp_result_as_string(ret)};
    }
}

GPhotoCamera::GPhotoCamera(const GPhotoCamera& other) noexcept {
    context = other.context;
    camera = other.camera;
}

GPhotoCamera::GPhotoCamera(GPhotoCamera&& other) noexcept {
    context = std::move(other.context);
    camera = other.camera;
    other.camera = nullptr;
}

GPhotoCamera::~GPhotoCamera() {}

GPhotoCamera& GPhotoCamera::operator=(const GPhotoCamera& other) noexcept {
    if (this == &other) {
        return *this;
    }

    context = other.context;
    camera = other.camera;
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

std::vector<std::filesystem::path> GPhotoCamera::list_files(const std::filesystem::path& path) const {
    return list_fs(false, path);
}

std::vector<std::filesystem::path> GPhotoCamera::list_folders(const std::filesystem::path& path) const {
    return list_fs(true, path);
}

std::vector<std::filesystem::path> GPhotoCamera::list_fs(bool folders, const std::filesystem::path& path) const {
    // For automatic clean up
    std::unique_ptr<CameraList, int (*)(CameraList*)> plist(nullptr, gp_list_free);

    {
        CameraList* list = nullptr;
        gp_list_new(&list);
        plist.reset(list);
    }

    int ret = 0;

    if (folders) {
        ret = gp_camera_folder_list_folders(camera.get(), path.c_str(), plist.get(), context.get_context());
    } else {
        ret = gp_camera_folder_list_files(camera.get(), path.c_str(), plist.get(), context.get_context());
    }

    if (ret < GP_OK) {
        if (folders) {
            std::cerr << "libgphoto2 gp_camera_folder_list_folders failed: ";
        } else {
            std::cerr << "libgphoto2 gp_camera_folder_list_files failed: ";
        }
        std::cerr << gp_result_as_string(ret) << std::endl;
        return {};
    }

    int fs_items_count = gp_list_count(plist.get());
    std::vector<std::filesystem::path> result;
    result.reserve(fs_items_count);

    for (int i = 0; i < fs_items_count; i++) {
        const char* item_name;
        gp_list_get_name(plist.get(), i, &item_name);
        result.emplace_back(path / item_name);
    }

    return result;
}

bool GPhotoCamera::get_file(const std::filesystem::path& file_path,
                            const std::filesystem::path& destination_file) const {
    int fd = open(destination_file.c_str(), O_CREAT | O_WRONLY, 0644);
    if (fd < 0) {
        std::cerr << "Can't create file " << destination_file << ": " << strerror(errno) << std::endl;
        return false;
    }

    CameraFile* file = nullptr;

    int ret = gp_file_new_from_fd(&file, fd);
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_file_new_from_fd failed: " << gp_result_as_string(ret) << std::endl;
        close(fd);
        return false;
    }

    auto parent = file_path.parent_path();
    auto filename = file_path.filename();
    ret = gp_camera_file_get(
            camera.get(), parent.c_str(), filename.c_str(), GP_FILE_TYPE_NORMAL, file, context.get_context());

    if (ret < GP_OK) {
        std::cerr << "libgphoto2 gp_file_new_from_fd failed: " << gp_result_as_string(ret) << std::endl;

        // remove output file
        gp_file_free(file);
        close(fd);
        remove(destination_file.c_str());
        return false;
    }

    gp_file_free(file);
    close(fd);

    return true;
}
