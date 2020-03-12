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
#ifndef IPHONE_PHOTO_DOWNLOAD_APP_H
#define IPHONE_PHOTO_DOWNLOAD_APP_H

#include <cstdint>
#include <string>
#include <filesystem>

#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2.h>

#include "context.h"
#include "gphoto_info.h"
#include "gphoto_camera.h"
#include "folder_pair.h"

class App {
public:
    App();
    void print_device_list();
    void list_files(size_t idx, const std::filesystem::path& path, bool recursive);

    void download_files(size_t idx,
                        const std::filesystem::path& source,
                        const std::filesystem::path& destination,
                        bool recursive);

private:
    CameraList* autodetect_cameras() const;

    GPhotoCamera open_camera(size_t idx);

    void print_folder_structure(const GPhotoCamera& camera, const std::filesystem::path& path, bool recursive);

    void do_download_file(const GPhotoCamera& camera,
                          const std::filesystem::path& source,
                          const std::filesystem::path& destination);

    void do_download_folder(const GPhotoCamera& camera,
                            const std::filesystem::path& source,
                            const std::filesystem::path& destination,
                            bool recursive);

    void enumerate_files(const GPhotoCamera& camera,
                         const std::filesystem::path& folder,
                         const std::filesystem::path& destination,
                         std::vector<FolderPair>& files,
                         bool recursive);

    void print_enumerating_files(size_t files_count, bool finish);

private:
    Context context;
    GPhotoInfo info{context};
};

#endif // IPHONE_PHOTO_DOWNLOAD_APP_H
