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

void Kernel::operator()(const std::vector<Argument> &args,
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
        std::visit(
            [&](auto &&arg) {
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
        std::visit(
            [&](auto &&arg) {
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

    std::visit(
        [&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Array>) {
                auto &indirect = arg;
                encoder->dispatchThreadgroups(indirect.buffer().ptr(), 0, _threads_per_thread_group);
            } else if constexpr (std::is_same_v<T, std::array<uint32_t, 3>>) {
                auto &thread_groups = arg;
                encoder->dispatchThreadgroups(MTL::Size{thread_groups[0], thread_groups[1], thread_groups[2]},
                                              _threads_per_thread_group);
            } else if constexpr (std::is_same_v<T, MTL::Size>) {
                auto &threads = arg;
                encoder->dispatchThreads(threads, _threads_per_thread_group);
            }
        },
        _dispatch_threads);
}

void Kernel::set_indirect_threads(const Array &array) {
    _dispatch_threads = array;
}

void Kernel::set_thread_groups(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) {
    _dispatch_threads = std::array<uint32_t, 3>{groups_x, groups_y, groups_z};
}

void Kernel::set_threads(uint32_t threads_x, uint32_t threads_y, uint32_t threads_z) {
    _dispatch_threads = MTL::Size{threads_x, threads_y, threads_z};
}

std::uintptr_t Kernel::max_total_threads_per_threadgroup() const {
    return _pso->maxTotalThreadsPerThreadgroup();
}

void Kernel::set_threads_per_thread_group(uint32_t threads_per_thread_group_x,
                                          uint32_t threads_per_thread_group_y,
                                          uint32_t threads_per_thread_group_z) {
    _threads_per_thread_group = MTL::Size{threads_per_thread_group_x,
                                          threads_per_thread_group_y,
                                          threads_per_thread_group_z};
}

}// namespace vox