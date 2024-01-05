//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_stream.h"
#include "metal_kernel.h"
#include "metal_buffer.h"

namespace vox {
MetalStream::MetalStream(MTL::Device *device)
    : _queue{device->newCommandQueue()} {
    _queue->retain();
}

MetalStream::~MetalStream() {
    _queue->release();
}

[[nodiscard]] inline auto align(size_t s, size_t a) noexcept {
    return (s + (a - 1)) & ~(a - 1);
}

void MetalStream::dispatch_thread_groups(
    std::shared_ptr<Kernel> kernel,
    std::array<uint32_t, 3> thread_groups_per_grid,
    std::array<uint32_t, 3> threads_per_thread_group,
    const std::vector<Argument> &args) {
    static constexpr auto argument_buffer_size = 65536u;
    static constexpr auto argument_alignment = 16u;
    static thread_local std::array<std::byte, argument_buffer_size> argument_buffer;

    // encode arguments
    auto argument_offset = static_cast<size_t>(0u);
    auto copy = [&argument_offset](const void *ptr, size_t size) mutable noexcept {
        argument_offset = align(argument_offset, argument_alignment);
        assert(argument_offset + size <= argument_buffer_size);
        std::memcpy(argument_buffer.data() + argument_offset, ptr, size);
        return argument_offset += size;
    };

    for (const auto &arg : args) {
        std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, UniformArgument>) {
                copy(arg.data(), arg.size());
            } else if constexpr (std::is_same_v<T, BufferArgument>) {
                auto metal_buffer = std::static_pointer_cast<MetalBuffer>(arg);
                auto binding = metal_buffer->address();
                copy(&binding, sizeof(binding));
            }
        },
                   arg);
    }

    auto metal_kernel = std::static_pointer_cast<MetalKernel>(kernel);
    auto cb = _queue->commandBuffer();
    auto compute_encoder = cb->computeCommandEncoder();
    compute_encoder->setComputePipelineState(metal_kernel->pso().get());
    compute_encoder->setBytes(argument_buffer.data(), argument_offset, 0u);
    compute_encoder->dispatchThreadgroups(MTL::Size{thread_groups_per_grid[0], thread_groups_per_grid[1], thread_groups_per_grid[2]},
                                          MTL::Size{threads_per_thread_group[0], threads_per_thread_group[1], threads_per_thread_group[2]});
    compute_encoder->endEncoding();
    cb->commit();
}

void MetalStream::synchronize() {
    auto command_buffer = _queue->commandBufferWithUnretainedReferences();
    command_buffer->commit();
    command_buffer->waitUntilCompleted();
}
}// namespace vox