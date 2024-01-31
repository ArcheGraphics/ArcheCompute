//  Copyright (c) 2024 Feng Yang
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
    MetalBuffer(MTL::Device *device, size_t element_stride, size_t size);

    ~MetalBuffer() override;

    [[nodiscard]] uint64_t address() const noexcept;

    [[nodiscard]] inline MTL::Buffer *handle() const {
        return _buffer;
    }

private:
    MTL::Buffer *_buffer{nullptr};
};
}// namespace vox