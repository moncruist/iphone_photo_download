#include <iostream>
#include <iomanip>
#include <libusb.h>

int main() {
    libusb_device **devs;
    libusb_context *ctx = nullptr;
    int ret = 0;
    ret = libusb_init(&ctx);
    if (ret < 0) {
        std::cout << "Init error: " << ret << std::endl;
        return 1;
    }

    libusb_set_debug(ctx, 3);

    auto count = libusb_get_device_list(ctx, &devs);
    if (count < 0) {
        std::cout << "Get device error" << std::endl;
        libusb_exit(ctx);
        return 1;
    }

    for (ssize_t i = 0; i < count; i++) {
        libusb_device *dev = devs[i];
        libusb_device_descriptor desc;

        if (libusb_get_device_descriptor(dev, &desc) < 0) {
            std::cout << "Failed to get device descriptor" << std::endl;
            continue;
        }

        std::cout << "Config nums: " << static_cast<int>(desc.bNumConfigurations) << std::endl;
        std::cout << "Device class: " << static_cast<int>(desc.bDeviceClass) << std::endl;
        std::cout << "Vendor ID: " << std::setw(4) << std::setfill('0') << std::hex << desc.idVendor << std::endl;
        std::cout << "Product ID: " << std::setw(4) << std::setfill('0') << std::hex << desc.idProduct << std::endl;
        std::cout << std::endl;
    }

    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);

    return 0;
}
