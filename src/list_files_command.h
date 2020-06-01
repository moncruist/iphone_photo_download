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
#ifndef PHCOPY_LIST_FILES_COMMAND_H
#define PHCOPY_LIST_FILES_COMMAND_H

#include "command.h"

#include <filesystem>

class ListFilesCommand : public Command {
public:
    ListFilesCommand(size_t device_idx, std::filesystem::path path, bool recursive);

    void execute() override;

private:
    void print_folder_structure(const GPhotoCamera& camera, const std::filesystem::path& path, bool recursive);

    size_t device_idx;
    std::filesystem::path path;
    bool recursive;
};

#endif // PHCOPY_LIST_FILES_COMMAND_H
