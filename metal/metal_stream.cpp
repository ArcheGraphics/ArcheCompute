//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_stream.h"
#include "metal_kernel.h"

namespace vox {
MetalStream::MetalStream(MTL::Device *device)
    : _queue{device->newCommandQueue()} {
    _queue->retain();
}

MetalStream::~MetalStream() {
    _queue->release();
}

void MetalStream::dispatch_thread_groups(
    std::shared_ptr<Kernel> kernel,
    std::array<uint32_t, 3> thread_groups_per_grid,
    std::array<uint32_t, 3> threads_per_thread_group,
    const std::vector<Argument> &args) {
    auto metal_kernel = std::static_pointer_cast<MetalKernel>(kernel);
    auto cb = _queue->commandBuffer();
    auto compute_encoder = cb->computeCommandEncoder();
    compute_encoder->dispatchThreadgroups(MTL::Size{thread_groups_per_grid[0], thread_groups_per_grid[1], thread_groups_per_grid[2]},
                                          MTL::Size{threads_per_thread_group[0], threads_per_thread_group[1], threads_per_thread_group[2]});
    cb->commit();
}

void MetalStream::submit() {
}
}// namespace vox