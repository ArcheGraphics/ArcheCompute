//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/buffer.h"
#include <Metal/Metal.hpp>

namespace vox {
class MetalBuffer : public Buffer {
public:
    MetalBuffer(MTL::Device *device, size_t size);

    ~MetalBuffer() override;

    [[nodiscard]] uint64_t address() const noexcept;

private:
    MTL::Buffer *_buffer{nullptr};
};
}// namespace vox