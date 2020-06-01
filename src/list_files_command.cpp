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
#include "list_files_command.h"

#include <iostream>

ListFilesCommand::ListFilesCommand(size_t device_idx, std::filesystem::path path, bool recursive)
  : device_idx(device_idx), path(std::move(path)), recursive(recursive) {}

void ListFilesCommand::execute() {
    try {
        GPhotoCamera camera = open_camera(device_idx);
        print_folder_structure(camera, path, recursive);
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void ListFilesCommand::print_folder_structure(const GPhotoCamera& camera,
                                              const std::filesystem::path& path,
                                              bool recursive) {
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
