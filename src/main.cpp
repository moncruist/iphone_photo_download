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
#include "app.h"

#include <boost/program_options.hpp>
#include <iomanip>
#include <iostream>
#include <variant>

namespace po = boost::program_options;

enum class Command { LIST_DEVICES, LIST_FILES, DOWNLOAD_FILES };

struct ListDevicesCommand {};

struct ListFilesCommand {
    int device_index;
    std::filesystem::path path;
    bool recursive;
};

struct DownloadCommand {
    int device_index;
    std::filesystem::path source;
    bool recursive;
    std::filesystem::path destination;
};

using Options = std::variant<ListDevicesCommand, ListFilesCommand, DownloadCommand>;

namespace {

inline const char* LIST_DEVICES_COMMAND = "list";
inline const char* LIST_FILES_COMMAND = "list-files";
inline const char* DOWNLOAD_FILES_COMMAND = "download";

const std::pair<const char*, Command> SUPPORTED_COMMANDS[] = {{LIST_DEVICES_COMMAND, Command::LIST_DEVICES},
                                                              {LIST_FILES_COMMAND, Command::LIST_FILES},
                                                              {DOWNLOAD_FILES_COMMAND, Command::DOWNLOAD_FILES}};

} // namespace

std::optional<Options> parse_options(int argc, char* argv[]) {
    po::options_description desc("All options");

    // clang-format off
    desc.add_options()
            ("command", po::value<std::string>()->required(), "Command")
            ("help", "produce help message")
            ("device", po::value<int>()->default_value(0), "Device number")
            ("subargs", po::value<std::vector<std::string> >(), "Arguments for command")
            ("recursive,r", "Recursive listing");
    // clang-format on

    po::positional_options_description positional;
    positional.add("command", 1).add("subargs", -1);

    po::variables_map vm;
    po::parsed_options parsed =
            po::command_line_parser(argc, argv).options(desc).positional(positional).allow_unregistered().run();
    po::store(parsed, vm);


    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return std::nullopt;
    }

    if (vm.count("command") == 0) {
        std::cerr << "Missing command" << std::endl;
        return std::nullopt;
    }

    const auto& command = vm["command"].as<std::string>();

    auto pos = std::find_if(std::begin(SUPPORTED_COMMANDS), std::end(SUPPORTED_COMMANDS), [&](const auto& elem) {
        return elem.first == command;
    });
    if (pos == std::end(SUPPORTED_COMMANDS)) {
        std::cerr << "Unknown command: " << command << std::endl;
        return std::nullopt;
    }

    if (command == LIST_FILES_COMMAND) {
        po::options_description ls_desc("list-files options");
        // clang-format off
        ls_desc.add_options()
            ("path", po::value<std::string>()->required(), "Path to list");
        // clang-format on

        po::positional_options_description list_files_positional;
        list_files_positional.add("path", 1);

        std::vector<std::string> opts = po::collect_unrecognized(parsed.options, po::include_positional);
        opts.erase(opts.begin());

        po::store(po::command_line_parser(opts).options(ls_desc).positional(list_files_positional).run(), vm);
    } else if (command == DOWNLOAD_FILES_COMMAND) {
        po::options_description ls_desc("download options");
        // clang-format off
        ls_desc.add_options()
                ("path", po::value<std::string>()->required(), "Path to list")
                ("destination", po::value<std::string>()->required(), "Path to list");
        // clang-format on

        po::positional_options_description list_files_positional;
        list_files_positional.add("path", 1);
        list_files_positional.add("destination", 1);

        std::vector<std::string> opts = po::collect_unrecognized(parsed.options, po::include_positional);
        opts.erase(opts.begin());

        po::store(po::command_line_parser(opts).options(ls_desc).positional(list_files_positional).run(), vm);
    }

    po::notify(vm);

    std::filesystem::path path, destination;
    if (vm.count("path") > 0) {
        path = vm["path"].as<std::string>();
    }

    if (vm.count("destination") > 0) {
        destination = vm["destination"].as<std::string>();
    }

    bool recursive = vm.count("recursive") > 0;

    if (command == LIST_DEVICES_COMMAND) {
        return ListDevicesCommand {};
    } else if (command == LIST_FILES_COMMAND) {
        return ListFilesCommand {vm["device"].as<int>(), path, recursive};
    } else {
        return DownloadCommand {vm["device"].as<int>(), path, recursive, destination};
    }
}

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

int main(int argc, char* argv[]) {
    std::optional<Options> options = parse_options(argc, argv);
    if (!options) {
        return 0;
    }

    try {
        App app;

        std::visit(overloaded {[&](const ListDevicesCommand&) { app.print_device_list(); },
                               [&](const ListFilesCommand& command) {
                                   app.list_files(command.device_index, command.path, command.recursive);
                               },
                               [&](const DownloadCommand& command) {
                                   app.download_files(command.device_index,
                                                      command.source,
                                                      command.destination,
                                                      command.recursive);
                               }},
                   *options);
    } catch (std::runtime_error& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
