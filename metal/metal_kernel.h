//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/kernel.h"
#include <Metal/Metal.hpp>

namespace vox {
class MetalCommandEncoder;
class MetalKernel : public Kernel {
public:
    MetalKernel(MTL::Device *device, const std::string &code, const std::string &entry);

    std::unique_ptr<ShaderDispatchCommand> launch_thread_groups(
        std::array<uint32_t, 3> thread_groups_per_grid,
        std::array<uint32_t, 3> threads_per_thread_group,
        std::vector<Argument> &&args) override;

    [[nodiscard]] inline const NS::SharedPtr<MTL::ComputePipelineState> &pso() const {
        return _pso;
    }

    void launch(MetalCommandEncoder &encoder, const ShaderDispatchCommand *command) const noexcept;

private:
    NS::SharedPtr<MTL::ComputePipelineState> _pso;
};
}// namespace vox