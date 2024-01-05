//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/stream.h"
#include <Metal/Metal.hpp>

namespace vox {
class MetalStream : public Stream {
public:
    explicit MetalStream(MTL::Device *device);

    ~MetalStream() override;

    void synchronize() override;

    void dispatch_thread_groups(
        std::shared_ptr<Kernel> kernel,
        std::array<uint32_t, 3> thread_groups_per_grid,
        std::array<uint32_t, 3> threads_per_thread_group,
        const std::vector<Argument> &args) override;

private:
    MTL::CommandQueue *_queue{nullptr};
};
}// namespace vox