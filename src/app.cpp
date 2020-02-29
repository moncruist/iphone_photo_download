#include "app.h"

#include "gphoto_camera.h"

#include <algorithm>
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2.h>
#include <iostream>
#include <cstdio>

App::App() : info(context) {
    if (!info.load_cameras_abilities()) {
        throw std::runtime_error("Failed to load camera capabilities");
    }

    if (!info.load_port_info()) {
        throw std::runtime_error("Failed to load information about ports");
    }
}

void App::print_device_list() {
    CameraList* list = autodetect_cameras();

    if (list == nullptr) {
        return;
    }

    int devices_num = gp_list_count(list);

    std::cout << "Found devices: " << devices_num << std::endl;

    for (int i = 0; i < devices_num; i++) {
        const char *name, *port;
        gp_list_get_name(list, i, &name);
        gp_list_get_value(list, i, &port);

        std::cout << "[" << i << "]: " << name << " / " << port << std::endl;
        std::cout << std::endl;
    }
    gp_list_free(list);
}


void App::list_files(size_t idx, const std::filesystem::path& path, bool recursive) {
    try {
        GPhotoCamera camera = open_camera(idx);
        print_folder_structure(camera, path, recursive);
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void App::download_files(size_t idx,
                         const std::filesystem::path& source,
                         const std::filesystem::path& destination,
                         bool recursive) {
    try {
        GPhotoCamera camera = open_camera(idx);
        if (source.has_filename()) {
            // might be the file
            auto source_parent = source.parent_path();

            auto folders = camera.list_folders(source_parent);
            auto folders_pos = std::find(folders.begin(), folders.end(), source);
            if (folders_pos != folders.end()) {
                // source is a folder
                do_download_folder(camera, source, destination, recursive);
            } else {
                auto files = camera.list_files(source_parent);
                auto files_pos = std::find(files.begin(), files.end(), source);

                if (files_pos == files.end()) {
                    std::cerr << "Can't find file " << source << std::endl;
                    return;
                }

                // source is a file
                do_download_file(camera, source, destination);
            }
        } else {
            // source is definitely a folder
            do_download_folder(camera, source, destination, true);
        }
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

CameraList* App::autodetect_cameras() const {
    CameraList* list = nullptr;
    gp_list_new(&list);

    int ret = gp_camera_autodetect(list, context.get_context());
    if (ret < GP_OK) {
        std::cerr << "libgphoto2 autodetect failed: " << gp_result_as_string(ret) << std::endl;
        return nullptr;
    }

    return list;
}

void App::print_folder_structure(const GPhotoCamera& camera, const std::filesystem::path& path, bool recursive) {
    auto folders = camera.list_folders(path);
    auto files = camera.list_files(path);

    for (const auto& folder : folders) {
        if (recursive) {
            print_folder_structure(camera, folder, true);
        } else {
            std::cout << folder << std::endl;
        }
    }
    for (const auto& file : files) {
        std::cout << file << std::endl;
    }
}

GPhotoCamera App::open_camera(size_t idx) {
    CameraList* list = autodetect_cameras();

    if (list == nullptr) {
        throw std::runtime_error {"No cameras available"};
    }

    int devices_num = gp_list_count(list);

    if (idx >= devices_num) {
        gp_list_free(list);
        throw std::runtime_error {"Device index is out of range"};
    }

    const char *name, *port;
    gp_list_get_name(list, idx, &name);
    gp_list_get_value(list, idx, &port);

    try {
        GPhotoCamera camera(name, port, context, info);
        gp_list_free(list);

        return camera;
    } catch (std::runtime_error& e) {
        gp_list_free(list);
        throw;
    }
}

void App::do_download_file(const GPhotoCamera& camera,
                           const std::filesystem::path& source,
                           const std::filesystem::path& destination) {
    std::cout << "Downloading " << source << "..." << std::flush;
    bool result = camera.get_file(source, destination);

    std::cout << (result ? "DONE" : "FAILED") << std::endl;
}

void App::do_download_folder(const GPhotoCamera& camera,
                             const std::filesystem::path& source,
                             const std::filesystem::path& destination,
                             bool recursive) {
    std::vector<std::filesystem::path> files;

    print_enumerating_files(files.size(), false);
    enumerate_files(camera, source, files, recursive);
    print_enumerating_files(files.size(), true);


}

void App::dowload_folder_step(const GPhotoCamera& camera,
                              const std::filesystem::path& source,
                              const std::filesystem::path& destination,
                              bool recursive) {}

void App::enumerate_files(const GPhotoCamera& camera,
                          const std::filesystem::path& folder,
                          std::vector<std::filesystem::path>& files,
                          bool recursive) {
    auto files_list = camera.list_files(folder);
    for (const auto& file_entry : files_list) {
        files.push_back(file_entry);
    }

    print_enumerating_files(files.size(), false);
    if (recursive) {
        auto folder_list = camera.list_folders(folder);
        for (const auto& dir : folder_list) {
            enumerate_files(camera, dir, files, recursive);
        }
    }
}

void App::print_enumerating_files(size_t files_count, bool finish) {
    std::cout << "\rEnumerating files: " <<  files_count << std::flush;
    if (finish) {
        puts("\n");
    }
}
