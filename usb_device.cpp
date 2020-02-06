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

#include "usb_device.h"

#include <iostream>

UsbDevice::UsbDevice(libusb_device* device) : device_(device)
{

}

std::optional<libusb_device_descriptor> UsbDevice::get_descriptor() const
{
    if (!device_) {
        return std::nullopt;
    }

    libusb_device_descriptor desc;
    int ret = libusb_get_device_descriptor(device_, &desc);
    if (ret < 0) {
        std::cerr << "Failed to get descriptor: " << libusb_error_name(ret) << std::endl;
        return std::nullopt;
    }

    return desc;
}
