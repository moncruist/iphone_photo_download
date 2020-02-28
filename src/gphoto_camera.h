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
#ifndef IPHONE_PHOTO_DOWNLOAD_GPHOTO_CAMERA_H
#define IPHONE_PHOTO_DOWNLOAD_GPHOTO_CAMERA_H

#include "context.h"
#include "gphoto_info.h"

#include <vector>
#include <string>
#include <filesystem>

class GPhotoCamera {
public:
    GPhotoCamera(const char* model, const char* port, Context context, const GPhotoInfo& info);
    GPhotoCamera(const GPhotoCamera& other) noexcept;
    GPhotoCamera(GPhotoCamera&& other) noexcept;
    ~GPhotoCamera();

    GPhotoCamera& operator=(const GPhotoCamera& other) noexcept;
    GPhotoCamera& operator=(GPhotoCamera&& other) noexcept;

    std::vector<std::filesystem::path> list_files(const std::filesystem::path& path);
    std::vector<std::filesystem::path> list_folders(const std::filesystem::path& path);

private:
    std::vector<std::filesystem::path> list_fs(bool folders, const std::filesystem::path& path);

    Context context; // For holding reference
    Camera* camera {nullptr};
};


#endif // IPHONE_PHOTO_DOWNLOAD_GPHOTO_CAMERA_H
