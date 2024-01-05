//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include "argument.h"
#include "kernel.h"

namespace vox {
class Stream {
public:
    virtual ~Stream() = default;

    virtual void dispatch_thread_groups(
        std::shared_ptr<Kernel> kernel,
        std::array<uint32_t, 3> thread_groups_per_grid,
        std::array<uint32_t, 3> threads_per_thread_group,
        const std::vector<Argument> &args) = 0;

    virtual void synchronize() = 0;

    virtual void dispatch(std::vector<std::unique_ptr<Command>> commands) = 0;
};

}// namespace vox