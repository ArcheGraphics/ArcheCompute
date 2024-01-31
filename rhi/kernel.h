//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/command.h"
#include "rhi/argument.h"

namespace vox {
class Kernel : public std::enable_shared_from_this<Kernel> {
public:
    Kernel() = default;
    virtual ~Kernel() = default;

    std::shared_ptr<ShaderDispatchCommand> launch_thread_groups(
        std::array<uint32_t, 3> thread_groups_per_grid,
        std::array<uint32_t, 3> threads_per_thread_group,
        const std::vector<Argument> &args);
};
}// namespace vox
