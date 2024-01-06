//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_kernel.h"
#include "metal_buffer.h"
#include "metal_command_encoder.h"
#include <string>

namespace vox {
MetalKernel::MetalKernel(MTL::Device *device, const std::string &code, const std::string &entry) {
    NS::Error *pError = nullptr;

    MTL::Library *pComputeLibrary = device->newLibrary(NS::String::string(code.c_str(), NS::UTF8StringEncoding), nullptr, &pError);
    if (!pComputeLibrary) {
        __builtin_printf("%s", pError->localizedDescription()->utf8String());
        assert(false);
    }

    MTL::Function *pMandelbrotFn = pComputeLibrary->newFunction(NS::String::string(entry.c_str(), NS::UTF8StringEncoding));
    _pso = NS::TransferPtr(device->newComputePipelineState(pMandelbrotFn, &pError));
    if (!_pso) {
        __builtin_printf("%s", pError->localizedDescription()->utf8String());
        assert(false);
    }

    pMandelbrotFn->release();
    pComputeLibrary->release();
}

std::unique_ptr<ShaderDispatchCommand> MetalKernel::launch_thread_groups(
    std::array<uint32_t, 3> thread_groups_per_grid,
    std::array<uint32_t, 3> threads_per_thread_group,
    std::vector<Argument> &&args) {
    return std::make_unique<ShaderDispatchCommand>(shared_from_this(), thread_groups_per_grid, threads_per_thread_group, std::move(args));
}

[[nodiscard]] inline auto align(size_t s, size_t a) noexcept {
    return (s + (a - 1)) & ~(a - 1);
}

void MetalKernel::launch(MetalCommandEncoder &encoder, const ShaderDispatchCommand *command) const noexcept {
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

    auto encode = [&](Argument arg) mutable noexcept {
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
    };

    auto mark_usage = [&, index = 0u](MTL::ComputeCommandEncoder *compute_encoder, Argument arg) mutable noexcept {
        std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, UniformArgument>) {
                return;
            } else if constexpr (std::is_same_v<T, BufferArgument>) {
                auto metal_buffer = std::static_pointer_cast<MetalBuffer>(arg);
                compute_encoder->useResource(metal_buffer->handle(), MTL::ResourceUsageWrite);
            }
        },
                   arg);
    };

    auto metal_kernel = std::static_pointer_cast<const MetalKernel>(command->handle());
    auto compute_encoder = encoder.command_buffer()->computeCommandEncoder(MTL::DispatchTypeConcurrent);
    compute_encoder->setComputePipelineState(metal_kernel->pso().get());

    for (const auto &arg : command->argument_buffer()) {
        encode(arg);
        mark_usage(compute_encoder, arg);
    }
    compute_encoder->setBytes(argument_buffer.data(), align(argument_offset, argument_alignment), 0u);

    auto thread_groups_per_grid = command->thread_groups_per_grid();
    auto threads_per_thread_group = command->threads_per_thread_group();
    compute_encoder->dispatchThreadgroups(MTL::Size{thread_groups_per_grid[0], thread_groups_per_grid[1], thread_groups_per_grid[2]},
                                          MTL::Size{threads_per_thread_group[0], threads_per_thread_group[1], threads_per_thread_group[2]});
    compute_encoder->endEncoding();
}

}// namespace vox