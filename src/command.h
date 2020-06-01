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
#ifndef PHCOPY_COMMAND_H
#define PHCOPY_COMMAND_H

#include "context.h"
#include "gphoto_info.h"
#include "gphoto_camera.h"

class Command {
public:
    Command();

    virtual void execute();

protected:
    void load_camera_info();
    Context& get_context();
    GPhotoInfo& get_gphoto_info();
    CameraList* autodetect_cameras() const;
    GPhotoCamera open_camera(size_t idx);

private:
    Context context;
    GPhotoInfo info {context};
};


#endif // PHCOPY_COMMAND_H
