//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "kernel.h"
#include "helpers.h"
#include "stream.h"
#include "array.h"

namespace vox {
Kernel::Builder Kernel::builder() { return {}; }

Kernel::Builder &Kernel::Builder::source(std::string code) {
    _source = std::move(code);
    return *this;
}
Kernel::Builder &Kernel::Builder::entry(std::string entry) {
    _base_name = std::move(entry);
    return *this;
}
Kernel::Builder &Kernel::Builder::lib_name(std::string name) {
    _lib_name = std::move(name);
    return *this;
}
Kernel::Builder &Kernel::Builder::hash_name(std::string name) {
    _hash_name = std::move(name);
    return *this;
}
Kernel::Builder &Kernel::Builder::func_consts(MTLFCList consts) {
    _func_consts = std::move(consts);
    return *this;
}
Kernel::Builder &Kernel::Builder::linked_functions(std::vector<MTL::Function *> functions) {
    _linked_functions = std::move(functions);
    return *this;
}

Kernel Kernel::Builder::build() const {
    if (!_source.empty()) {
        device().get_library(_lib_name, _source);
    }

    auto pso = device().get_kernel(_base_name, _lib_name, _hash_name, _func_consts, _linked_functions);
    return Kernel(pso);
}

Kernel::Kernel(MTL::ComputePipelineState *pso) : _pso{pso} {}

void Kernel::operator()(
    std::array<uint32_t, 3> thread_groups_per_grid,
    std::array<uint32_t, 3> threads_per_thread_group,
    const std::initializer_list<Argument> &args,
    uint32_t stream) {

    static constexpr auto argument_buffer_size = 65536u;
    static constexpr auto argument_alignment = 8u;
    static thread_local std::array<std::byte, argument_buffer_size> argument_buffer;

    // encode arguments
    auto argument_offset = static_cast<size_t>(0u);
    auto copy = [&argument_offset](const void *ptr, size_t size) mutable noexcept {
        argument_offset = align(argument_offset, argument_alignment);
        assert(argument_offset + size <= argument_buffer_size);
        std::memcpy(argument_buffer.data() + argument_offset, ptr, size);
        return argument_offset += size;
    };

    auto encode = [&](const Argument &arg) mutable noexcept {
        std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, UniformArgument>) {
                copy(arg.data(), arg.size());
            } else if constexpr (std::is_same_v<T, ArrayArgument>) {
                auto binding = arg.buffer().address();
                copy(&binding, sizeof(binding));
            }
        },
                   arg);
    };

    auto mark_usage = [&, index = 0u](MTL::ComputeCommandEncoder *compute_encoder, const Argument &arg) mutable noexcept {
        std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, UniformArgument>) {
                return;
            } else if constexpr (std::is_same_v<T, ArrayArgument>) {
                compute_encoder->useResource(arg.buffer().ptr(), MTL::ResourceUsageWrite | MTL::ResourceUsageRead);
            }
        },
                   arg);
    };

    auto &s = device().stream(stream);
    auto encoder = s.get_command_encoder();
    encoder->setComputePipelineState(_pso);
    for (const Argument &arg : args) {
        encode(arg);
        mark_usage(encoder, arg);
    }
    encoder->setBytes(argument_buffer.data(), align(argument_offset, argument_alignment), 0u);
    encoder->dispatchThreadgroups(MTL::Size{thread_groups_per_grid[0], thread_groups_per_grid[1], thread_groups_per_grid[2]},
                                  MTL::Size{threads_per_thread_group[0], threads_per_thread_group[1], threads_per_thread_group[2]});
}

}// namespace vox