#include "app.h"

#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2.h>
#include <iostream>

App::App() {
    context = gp_context_new();
}

App::~App() {
    if (context != nullptr) {
        gp_context_unref(context);
    }
}

App::App(const App& other) : context(other.context) {
    gp_context_ref(context);
}

App::App(App&& other) : context(other.context) {
    other.context = nullptr;
}

App& App::operator=(const App& other) {
    context = other.context;
    gp_context_ref(context);
    return *this;
}

App& App::operator=(App&& other) {
    context = other.context;
    other.context = nullptr;
    return *this;
}


void App::print_device_list() {
    CameraList* list = nullptr;
    gp_list_new(&list);

    gp_camera_autodetect(list, context);

    int devices_num = gp_list_count(list);

    std::cout << "Found devices: " << devices_num << std::endl;

    for (int i = 0; i < devices_num; i++) {
        const char *name, *value;
        gp_list_get_name(list, i, &name);
        gp_list_get_value(list, i, &value);

        std::cout << "Name: " << name << std::endl;
        std::cout << "Value: " << value << std::endl;
        std::cout << std::endl;
    }
    gp_list_free(list);
}
