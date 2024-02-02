//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "device.h"
#include "argument.h"

namespace vox {

class Kernel {
public:
    class Builder;
    static Builder builder();

    void operator()(std::array<uint32_t, 3> thread_groups_per_grid,
                    std::array<uint32_t, 3> threads_per_thread_group,
                    const std::vector<Argument> &args,
                    uint32_t stream = 0);

private:
    explicit Kernel(MTL::ComputePipelineState *pso);
    MTL::ComputePipelineState *_pso;
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