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
#include "download_command.h"

#include <algorithm>
#include <iostream>

DownloadCommand::DownloadCommand(size_t device_idx,
                                 std::filesystem::path source,
                                 std::filesystem::path destination,
                                 bool recursive,
                                 bool skip_existing)
  : device_idx(device_idx),
    source(std::move(source)),
    destination(std::move(destination)),
    recursive(recursive),
    skip_existing(skip_existing) {}

void DownloadCommand::execute() {
    try {
        Command::execute();

        GPhotoCamera camera = open_camera(device_idx);
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

void DownloadCommand::do_download_file(const GPhotoCamera& camera,
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

void DownloadCommand::do_download_folder(const GPhotoCamera& camera,
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

void DownloadCommand::enumerate_files(const GPhotoCamera& camera,
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

void DownloadCommand::print_enumerating_files(size_t files_count, bool finish) {
    std::cout << "\rEnumerating files: " << files_count << std::flush;
    if (finish) {
        puts("\n");
    }
}
