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

namespace po = boost::program_options;

enum class Command { LIST_DEVICES, LIST_FILES };

struct Options {
    Command command;
    int device_index;
};

namespace {

const std::pair<const char*, Command> SUPPORTED_COMMANDS[] = {{"list", Command::LIST_DEVICES},
                                                              {"list-files", Command::LIST_FILES}};

}

std::optional<Options> parse_options(int argc, char* argv[]) {
    po::options_description desc("All options");

    // clang-format off
    desc.add_options()
            ("command", po::value<std::string>()->required(), "Command")
            ("help", "produce help message")
            ("device", po::value<int>()->default_value(0), "Device number");
    // clang-format on

    po::positional_options_description positional;
    positional.add("command", 1);

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).positional(positional).run();
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

    po::notify(vm);

    Options options {pos->second, vm["device"].as<int>()};
    return options;
}


int main(int argc, char* argv[]) {
    std::optional<Options> options = parse_options(argc, argv);
    if (!options) {
        return 0;
    }

    App app;

    switch (options->command) {
        case Command::LIST_DEVICES:
            app.print_device_list();
            break;

        case Command::LIST_FILES:
            app.open_camera(options->device_index);
            break;
    }

    return 0;
}
