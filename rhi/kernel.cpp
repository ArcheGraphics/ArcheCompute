//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "kernel.h"

namespace vox {
std::shared_ptr<ShaderDispatchCommand> Kernel::launch_thread_groups(
    std::array<uint32_t, 3> thread_groups_per_grid,
    std::array<uint32_t, 3> threads_per_thread_group,
    const std::vector<Argument> &args) {
    return std::make_shared<ShaderDispatchCommand>(shared_from_this(), thread_groups_per_grid, threads_per_thread_group, args);
}
}// namespace vox