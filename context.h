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
#ifndef CONTEXT_H
#define CONTEXT_H

#include <gphoto2/gphoto2-context.h>

class Context {
public:
    Context() noexcept;
    explicit Context(GPContext* context) noexcept;
    ~Context();
    Context(const Context& other) noexcept;
    Context(Context&& other) noexcept;
    Context& operator=(const Context& other) noexcept;
    Context& operator=(Context&& other) noexcept;

    GPContext* get_context() const noexcept;

private:
    GPContext* context {nullptr};
};

#endif // CONTEXT_H
