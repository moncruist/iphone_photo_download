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

UsbDevice::UsbDevice(libusb_device& device) : device_(&device) {
    libusb_ref_device(device_);
}

UsbDevice::UsbDevice(const UsbDevice& other) : device_(other.device_), handle_(other.handle_) {
    libusb_ref_device(device_);
}

UsbDevice::UsbDevice(UsbDevice&& other) : device_(other.device_), handle_(other.handle_) {}

UsbDevice::~UsbDevice() {
    if (is_open()) {
        close();
    }
    libusb_unref_device(device_);
}

UsbDevice& UsbDevice::operator=(const UsbDevice& other) {
    if (this != &other) {
        device_ = other.device_;
        handle_ = other.handle_;

        libusb_ref_device(device_);
    }

    return *this;
}

UsbDevice& UsbDevice::operator=(UsbDevice&& other) {
    if (this != &other) {
        device_ = other.device_;
        handle_ = other.handle_;
    }

    return *this;
}

std::optional<libusb_device_descriptor> UsbDevice::get_descriptor() const {
    libusb_device_descriptor desc;
    int ret = libusb_get_device_descriptor(device_, &desc);
    if (ret < 0) {
        std::cerr << "Failed to get descriptor: " << libusb_error_name(ret) << std::endl;
        return std::nullopt;
    }

    return desc;
}

bool UsbDevice::is_open() {
    return handle_ != nullptr;
}

bool UsbDevice::open() {
    if (is_open()) {
        return false; // Already opened
    }

    int ret = libusb_open(device_, &handle_);
    if (ret != 0) {
        std::cerr << "Failed to open device: " << libusb_error_name(ret) << std::endl;
        return false;
    }

    return true;
}

bool UsbDevice::close() {
    if (!is_open()) {
        return false; // Already closed
    }

    libusb_close(handle_);
    handle_ = nullptr;
    return true;
}
