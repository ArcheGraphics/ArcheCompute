//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "command.h"

namespace vox {

class CommandList {
public:
    using CommandContainer = std::vector<std::unique_ptr<Command>>;

private:
    CommandContainer _commands;

public:
    CommandList() noexcept = default;
    ~CommandList() noexcept;
    CommandList(CommandList &&another) noexcept;
    CommandList &operator=(CommandList &&rhs) noexcept = delete;
    [[nodiscard]] static CommandList create(size_t reserved_command_size = 0u) noexcept;
    void reserve(size_t command_size) noexcept;

    CommandList &operator<<(std::unique_ptr<Command> &&cmd) noexcept;
    CommandList &append(std::unique_ptr<Command> &&cmd) noexcept;
    void clear() noexcept;

    [[nodiscard]] CommandContainer commands() noexcept;
    [[nodiscard]] auto empty() const noexcept { return _commands.empty(); }
};

}// namespace vox
