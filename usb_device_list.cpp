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

#include "usb_device_list.h"

#include <iostream>

UsbDeviceList::UsbDeviceList(libusb_context& context)
{
    ssize_t cnt = libusb_get_device_list(&context, &devices_);
    if (cnt < 0) {
        std::cout << "Failed to enumerate devices: " << libusb_error_name(cnt) << std::endl;
        devices_ = nullptr;
        return;
    }

    size_ = cnt;
}

UsbDeviceList::UsbDeviceList(UsbDeviceList&& other)
{
    devices_ = other.devices_;
    size_ = other.size_;
    other.devices_ = nullptr;
    other.size_ = 0;
}

UsbDeviceList::~UsbDeviceList()
{
    if (devices_) {
        libusb_free_device_list(devices_, 1);
    }
}

size_t UsbDeviceList::size() const
{
    return size_;
}

libusb_device* UsbDeviceList::operator[](size_t idx)
{
    return devices_[idx];
}
