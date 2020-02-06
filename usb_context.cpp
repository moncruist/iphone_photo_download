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

#include "usb_context.h"

#include <iostream>

UsbContext::UsbContext(bool debug) {
    int ret = libusb_init(&ctx_);
    if (ret < 0) {
        std::cerr << "Failed to init USB context: " << libusb_error_name(ret) << std::endl;
        ctx_ = nullptr;
        return;
    }

    if (debug) {
        libusb_set_option(ctx_, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
    }
}

UsbContext::~UsbContext() {
    if (ctx_) {
        libusb_exit(ctx_);
    }
}

std::optional<UsbDeviceList> UsbContext::enumerate_devices() const
{
    if (!ctx_) {
        return std::nullopt;
    }

    return UsbDeviceList(ctx_);
}
