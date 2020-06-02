// phcopy
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
#include "download_command.h"
#include "list_devices_command.h"
#include "list_files_command.h"

#include <boost/program_options.hpp>
#include <iomanip>
#include <iostream>
#include <variant>

namespace po = boost::program_options;

enum class command { LIST_DEVICES, LIST_FILES, DOWNLOAD_FILES };

struct ListDevicesCommandParameters {};

struct ListFilesCommandParameters {
    int device_index {-1};
    std::filesystem::path path;
    bool recursive {false};
};

struct DownloadCommandParameters {
    int device_index {-1};
    std::filesystem::path source;
    std::filesystem::path destination;
    bool recursive {false};
    bool skip {false};
};

using Options = std::variant<ListDevicesCommandParameters, ListFilesCommandParameters, DownloadCommandParameters>;

namespace {

inline const char* LIST_DEVICES_COMMAND = "list";
inline const char* LIST_FILES_COMMAND = "list-files";
inline const char* DOWNLOAD_FILES_COMMAND = "download";

const std::pair<const char*, command> SUPPORTED_COMMANDS[] = {{LIST_DEVICES_COMMAND, command::LIST_DEVICES},
                                                              {LIST_FILES_COMMAND, command::LIST_FILES},
                                                              {DOWNLOAD_FILES_COMMAND, command::DOWNLOAD_FILES}};

// clang-format off
inline const char* HELP_STRING = ""
"Usage: phcopy COMMAND [PARAMETER...]\n"
"\n"
"Commands:\n"
"        list                          Display connected devices\n"
"        list-files PATH               Display files on the device located in\n"
"                                      the specific path\n"
"        download SOURCE DESTINATION   Download files from SOURCE on\n"
"                                      the device to DESTINATION\n"
"                                      DESTINATION folder must exists\n"
"\n"
"Parameters:\n"
"        -d, --device NUMBER           Use device NUMBER. Default is 0\n"
"        -h, --help                    Print this help\n"
"        -r, --recursive               Recursive traverse directories\n"
"                                      (applies for list-files and\n"
"                                      download commands)\n"
"        -s, --skip                    Don't overwrite files\n";
// clang-format on
} // namespace

void print_help() {
    std::cout << HELP_STRING << std::endl;
}

std::optional<Options> parse_options(int argc, char* argv[]) {
    po::options_description desc("All options");

    // clang-format off
    desc.add_options()
            ("command", po::value<std::string>()->required(), "")
            ("help,h", "")
            ("device,d", po::value<int>()->default_value(0), "")
            ("subargs", po::value<std::vector<std::string> >(), "")
            ("recursive,r", "")
            ("skip,s", "");
    // clang-format on

    po::positional_options_description positional;
    positional.add("command", 1).add("subargs", -1);

    po::variables_map vm;
    po::parsed_options parsed =
            po::command_line_parser(argc, argv).options(desc).positional(positional).allow_unregistered().run();
    po::store(parsed, vm);


    if (vm.count("help")) {
        print_help();
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

        if (vm.count("path") == 0) {
            std::cerr << "Path is missing" << std::endl;
            return std::nullopt;
        }
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

        if (vm.count("path") == 0) {
            std::cerr << "Path is missing" << std::endl;
            return std::nullopt;
        }

        if (vm.count("destination") == 0) {
            std::cerr << "Destination is missing" << std::endl;
            return std::nullopt;
        }
    }

    std::filesystem::path path, destination;
    if (vm.count("path") > 0) {
        path = vm["path"].as<std::string>();
    }

    if (vm.count("destination") > 0) {
        destination = vm["destination"].as<std::string>();
    }

    bool recursive = vm.count("recursive") > 0;
    bool skip = vm.count("skip") > 0;

    if (command == LIST_DEVICES_COMMAND) {
        return ListDevicesCommandParameters {};
    } else if (command == LIST_FILES_COMMAND) {
        return ListFilesCommandParameters {vm["device"].as<int>(), path, recursive};
    } else {
        return DownloadCommandParameters {vm["device"].as<int>(), path, destination, recursive, skip};
    }
}

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

int main(int argc, char* argv[]) {
    std::optional<Options> options = parse_options(argc, argv);
    if (!options) {
        return 0;
    }

    try {
        std::unique_ptr<Command> command;
        std::visit(overloaded {[&](const ListDevicesCommandParameters&) {
                                   command = std::make_unique<ListDevicesCommand>();
                               },
                               [&](const ListFilesCommandParameters& params) {
                                   command = std::make_unique<ListFilesCommand>(
                                           params.device_index, params.path, params.recursive);
                               },
                               [&](const DownloadCommandParameters& params) {
                                   command = std::make_unique<DownloadCommand>(params.device_index,
                                                                               params.source,
                                                                               params.destination,
                                                                               params.recursive,
                                                                               params.skip);
                               }},
                   *options);
        if (command) {
            command->execute();
        }
    } catch (std::runtime_error& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
