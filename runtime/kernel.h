//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "device.h"
#include "argument.h"
#include "array.h"

namespace vox {

class Kernel {
public:
    class Builder;
    static Builder builder();

    [[nodiscard]] std::uintptr_t max_total_threads_per_threadgroup() const;

    void set_indirect_threads(const Array &array);

    void set_thread_groups(uint32_t groups_x, uint32_t groups_y = 1, uint32_t groups_z = 1);

    void set_threads(uint32_t threads_x, uint32_t threads_y = 1, uint32_t threads_z = 1);

    void set_threads_per_thread_group(uint32_t threads_per_thread_group_x,
                                      uint32_t threads_per_thread_group_y = 1,
                                      uint32_t threads_per_thread_group_z = 1);

    void operator()(const std::vector<Argument> &args,
                    uint32_t stream = 0);

private:
    explicit Kernel(MTL::ComputePipelineState *pso);
    MTL::ComputePipelineState *_pso;
    // none, indirect, thread_groups_per_grid, threads_per_grid
    std::variant<std::monostate, Array, std::array<uint32_t, 3>, MTL::Size> _dispatch_threads;
    MTL::Size _threads_per_thread_group{1, 1, 1};
};

class Kernel::Builder {
public:
    Kernel::Builder &source(std::string code);
    Kernel::Builder &entry(std::string entry);
    Kernel::Builder &lib_name(std::string name);
    Kernel::Builder &hash_name(std::string name);
    Kernel::Builder &func_consts(MTLFCList consts);
    Kernel::Builder &linked_functions(std::vector<MTL::Function *> functions);

    [[nodiscard]] Kernel build() const;

private:
    std::string _source;
    std::string _base_name;
    std::string _lib_name = "metal_kernel";
    std::string _hash_name;
    MTLFCList _func_consts = {};
    std::vector<MTL::Function *> _linked_functions = {};
};

}// namespace vox