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

#include <iostream>

Ptp::Ptp()
{

}

bool Ptp::support_ptp(libusb_device* device)
{
    if (!device) {
        return false;
    }

    libusb_device_descriptor desc;
    int ret = libusb_get_device_descriptor(device, &desc);
    if (ret<0) {
        std::cerr << "Failed to get descriptor: " << libusb_error_name(ret) << std::endl;
        return false;
    }

    bool supported = false;

    for (uint8_t i = 0; i<desc.bNumConfigurations; i++) {
        libusb_config_descriptor* conf;

        ret = libusb_get_config_descriptor(device, i, &conf);
        if (ret!=0) {
            continue;
        }

        for (uint8_t j = 0; j<conf->bNumInterfaces; j++) {
            const libusb_interface& interface = conf->interface[j];

            for (uint8_t k = 0; k<interface.num_altsetting; k++) {
                const libusb_interface_descriptor& if_desc = interface.altsetting[k];
                if (if_desc.bInterfaceClass==CAPTURE_DEVICE_INTERFACE
                        && if_desc.bInterfaceSubClass==STILL_IMAGE_SUBCLASS
                        && if_desc.bInterfaceProtocol==PTP_PROTOCOL) {
                    supported = true;
                    goto loop_end;
                }
            }
        }

        loop_end:
        libusb_free_config_descriptor(conf);
        if (supported) {
            return true;
        }
    }

    return false;
}
