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
#ifndef PHCOPY_DOWNLOAD_COMMAND_H
#define PHCOPY_DOWNLOAD_COMMAND_H

#include "command.h"

#include <filesystem>
#include <vector>

#include "folder_pair.h"

class DownloadCommand : public Command {
public:
    DownloadCommand(size_t device_idx,
                    std::filesystem::path source,
                    std::filesystem::path destination,
                    bool recursive,
                    bool skip_existing);

    void execute() override;

private:
    void do_download_file(const GPhotoCamera& camera,
                          const std::filesystem::path& source,
                          const std::filesystem::path& destination,
                          bool skip_existing);

    void do_download_folder(const GPhotoCamera& camera,
                            const std::filesystem::path& source,
                            const std::filesystem::path& destination,
                            bool recursive,
                            bool skip_existing);

    void enumerate_files(const GPhotoCamera& camera,
                         const std::filesystem::path& folder,
                         const std::filesystem::path& destination,
                         std::vector<FolderPair>& files,
                         bool recursive,
                         bool skip_existing);

    void print_enumerating_files(size_t files_count, bool finish);

    size_t device_idx;
    std::filesystem::path source;
    std::filesystem::path destination;
    bool recursive;
    bool skip_existing;
};


#endif // PHCOPY_DOWNLOAD_COMMAND_H
