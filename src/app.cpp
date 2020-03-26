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
#include "app.h"

#include "folder_pair.h"
#include "gphoto_camera.h"

#include <algorithm>
#include <cstdio>
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


void App::list_files(size_t idx, const std::filesystem::path& path, bool recursive) {
    try {
        GPhotoCamera camera = open_camera(idx);
        print_folder_structure(camera, path, recursive);
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void App::download_files(size_t idx,
                         const std::filesystem::path& source,
                         const std::filesystem::path& destination,
                         bool recursive,
                         bool skip_existing) {
    try {
        GPhotoCamera camera = open_camera(idx);
        if (source.has_filename()) {
            // might be the file
            auto source_parent = source.parent_path();

            auto folders = camera.list_folders(source_parent);
            auto folders_pos = std::find(folders.begin(), folders.end(), source);
            if (folders_pos != folders.end()) {
                // source is a folder
                do_download_folder(camera, source, destination, recursive, skip_existing);
            } else {
                auto files = camera.list_files(source_parent);
                auto files_pos = std::find(files.begin(), files.end(), source);

                if (files_pos == files.end()) {
                    std::cerr << "Can't find file " << source << std::endl;
                    return;
                }

                // source is a file
                do_download_file(camera, source, destination, skip_existing);
            }
        } else {
            // source is definitely a folder
            do_download_folder(camera, source, destination, recursive, skip_existing);
        }
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
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

void App::print_folder_structure(const GPhotoCamera& camera, const std::filesystem::path& path, bool recursive) {
    auto folders = camera.list_folders(path);
    auto files = camera.list_files(path);

    for (const auto& folder : folders) {
        if (recursive) {
            print_folder_structure(camera, folder, true);
        } else {
            std::cout << folder << std::endl;
        }
    }
    for (const auto& file : files) {
        std::cout << file << std::endl;
    }
}

GPhotoCamera App::open_camera(size_t idx) {
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

void App::do_download_file(const GPhotoCamera& camera,
                           const std::filesystem::path& source,
                           const std::filesystem::path& destination,
                           bool skip_existing) {
    std::cout << "Downloading " << source << "... " << std::flush;
    auto filename = source.filename();
    auto dest_path = destination / filename;
    if (skip_existing && std::filesystem::exists(dest_path)) {
        std::cout << "SKIPPED" << std::endl;
        return;
    }

    bool result = camera.get_file(source, destination / filename);

    std::cout << (result ? "DONE" : "FAILED") << std::endl;
}

void App::do_download_folder(const GPhotoCamera& camera,
                             const std::filesystem::path& source,
                             const std::filesystem::path& destination,
                             bool recursive,
                             bool skip_existing) {
    std::vector<FolderPair> files;

    if (!std::filesystem::exists(destination)) {
        std::cerr << "Folder doesn't exist: " << destination << std::endl;
        return;
    }

    print_enumerating_files(files.size(), false);
    enumerate_files(camera, source, destination, files, recursive, skip_existing);
    print_enumerating_files(files.size(), true);

    for (size_t i = 0; i < files.size(); i++) {
        const FolderPair& file_task = files[i];
        if (!std::filesystem::exists(file_task.destination)) {
            std::filesystem::create_directories(file_task.destination);
        }

        std::cout << "[" << (i + 1) << "/" << files.size() << "]: ";
        do_download_file(camera, file_task.source, file_task.destination, skip_existing);
    }
}

void App::enumerate_files(const GPhotoCamera& camera,
                          const std::filesystem::path& folder,
                          const std::filesystem::path& destination,
                          std::vector<FolderPair>& files,
                          bool recursive,
                          bool skip_existing) {
    auto files_list = camera.list_files(folder);
    for (const auto& file_entry : files_list) {
        bool append = true;
        if (skip_existing) {
            append = !std::filesystem::exists(destination / file_entry.filename());
        }

        if (append) {
            files.emplace_back(file_entry, destination);
        }
    }

    print_enumerating_files(files.size(), false);
    if (recursive) {
        auto folder_list = camera.list_folders(folder);
        for (const auto& dir : folder_list) {
            auto dir_name = *(--dir.end());
            enumerate_files(camera, dir, destination / dir_name, files, recursive, skip_existing);
        }
    }
}

void App::print_enumerating_files(size_t files_count, bool finish) {
    std::cout << "\rEnumerating files: " << files_count << std::flush;
    if (finish) {
        puts("\n");
    }
}
