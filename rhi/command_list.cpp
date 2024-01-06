//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "command_list.h"
#include "common/logging.h"

namespace vox {

CommandList::~CommandList() noexcept {
    ASSERT(empty(),
           "Destructing non-empty command list. "
           "Did you forget to commit?");
}

void CommandList::reserve(size_t command_size) noexcept {
    if (command_size) { _commands.reserve(command_size); }
}

void CommandList::clear() noexcept {
    _commands.clear();
}

CommandList &CommandList::append(std::unique_ptr<Command> &&cmd) noexcept {
    if (cmd) { _commands.emplace_back(std::move(cmd)); }
    return *this;
}

CommandList &CommandList::operator<<(std::unique_ptr<Command> &&cmd) noexcept {
    return append(std::move(cmd));
}

CommandList::CommandContainer CommandList::commands() noexcept {
    return std::move(_commands);
}

CommandList CommandList::create(size_t reserved_command_size) noexcept {
    CommandList list{};
    list.reserve(reserved_command_size);
    return list;
}

CommandList::CommandList(CommandList &&another) noexcept
    : _commands{std::move(another._commands)} {}

}// namespace vox
