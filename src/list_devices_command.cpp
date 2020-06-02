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
#include "list_devices_command.h"

#include <iostream>

void ListDevicesCommand::execute() {
    Command::execute();
    
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
