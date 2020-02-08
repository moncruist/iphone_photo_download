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
#include <iostream>
#include <iomanip>
#include <libusb.h>

#include "usb_context.h"
#include "ptp.h"

int main() {
    UsbContext context(false);
    auto devices = context.enumerate_devices().value();

    for (size_t i = 0; i < devices.size(); i++) {
        auto device = devices[i];

        libusb_device_descriptor desc;
        int ret = libusb_get_device_descriptor(device, &desc);
        if (ret < 0) {
            std::cerr << "Failed to get descriptor: " << libusb_error_name(ret) << std::endl;
            continue;
        }


        std::cout << "Config nums: " << static_cast<int>(desc.bNumConfigurations) << std::endl;
        std::cout << "Device class: " << static_cast<int>(desc.bDeviceClass) << std::endl;
        std::cout << "Vendor ID: " << std::setw(4) << std::setfill('0') << std::hex << desc.idVendor << std::endl;
        std::cout << "Product ID: " << std::setw(4) << std::setfill('0') << std::hex << desc.idProduct << std::endl;
        std::cout << "PTP supported: " << Ptp::support_ptp(device) << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
