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
#ifndef PHCOPY_GPHOTO_INFO_H
#define PHCOPY_GPHOTO_INFO_H

#include "context.h"

#include <gphoto2/gphoto2.h>

#include <memory>

class GPhotoInfo {
public:
    explicit GPhotoInfo(Context context) noexcept;
    ~GPhotoInfo();

    GPhotoInfo(const GPhotoInfo&) = delete;
    GPhotoInfo(GPhotoInfo&&) = delete;
    GPhotoInfo& operator=(const GPhotoInfo&) = delete;
    GPhotoInfo& operator=(GPhotoInfo&&) = delete;

    bool load_port_info() noexcept;
    bool load_cameras_abilities() noexcept;

    bool lookup_camera_ability(const char* model, CameraAbilities& abilities_out) const noexcept;
    bool lookup_port_path(const char* port, GPPortInfo& port_info) const noexcept;

private:
    Context context;
    std::unique_ptr<GPPortInfoList, int(*)(GPPortInfoList*)> port_info_list;
    std::unique_ptr<CameraAbilitiesList, int(*)(CameraAbilitiesList*)> abilities;
};


#endif // PHCOPY_GPHOTO_INFO_H
