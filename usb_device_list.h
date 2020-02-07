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

#ifndef IPHONE_PHOTO_DOWNLOAD_USB_DEVICE_LIST_H
#define IPHONE_PHOTO_DOWNLOAD_USB_DEVICE_LIST_H

#include <cstdint>
#include <libusb.h>
#include "usb_device.h"

class UsbDeviceList {
public:
    explicit UsbDeviceList(libusb_context& context);
    UsbDeviceList(UsbDeviceList&& other);
    ~UsbDeviceList();

    size_t size() const;
    UsbDevice operator[](size_t idx);

private:
    libusb_device** devices_{nullptr};
    size_t size_{0};
};

#endif //IPHONE_PHOTO_DOWNLOAD_USB_DEVICE_LIST_H
