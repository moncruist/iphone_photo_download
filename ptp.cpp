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
#include "ptp.h"

#include <algorithm>
#include <iostream>

Ptp::Ptp() {}

bool Ptp::support_ptp(libusb_device* device) {
    if (!device) {
        return false;
    }

    libusb_device_descriptor desc;
    int ret = libusb_get_device_descriptor(device, &desc);
    if (ret < 0) {
        std::cerr << "Failed to get descriptor: " << libusb_error_name(ret) << std::endl;
        return false;
    }

    bool supported = false;

    for (uint8_t i = 0; i < desc.bNumConfigurations; i++) {
        libusb_config_descriptor* conf;

        ret = libusb_get_config_descriptor(device, i, &conf);
        if (ret != 0) {
            continue;
        }

        supported = find_interface(conf);

        libusb_free_config_descriptor(conf);
        if (supported) {
            return true;
        }
    }

    return false;
}

bool Ptp::find_interface(libusb_config_descriptor* config) {
    return std::find_if(&config->interface[0],
                        &config->interface[config->bNumInterfaces],
                        [&](const libusb_interface& interface) -> bool {
                            return std::find_if(&interface.altsetting[0],
                                                &interface.altsetting[interface.num_altsetting],
                                                [&](const libusb_interface_descriptor& if_desc) -> bool {
                                                    return if_desc.bInterfaceClass == CAPTURE_DEVICE_INTERFACE &&
                                                           if_desc.bInterfaceSubClass == STILL_IMAGE_SUBCLASS &&
                                                           if_desc.bInterfaceProtocol == PTP_PROTOCOL;
                                                }) != &interface.altsetting[interface.num_altsetting];
                        }) != &config->interface[config->bNumInterfaces];
}
