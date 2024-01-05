//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_buffer.h"

namespace vox {
MetalBuffer::MetalBuffer(MTL::Device *device)
    : _buffer{device->newBuffer(0, MTL::ResourceOptions{})} {
    _buffer->retain();
}

MetalBuffer::~MetalBuffer() {
    _buffer->release();
}

MetalBuffer::Binding MetalBuffer::binding(size_t offset) const noexcept {
    return {_buffer->gpuAddress() + offset};
}

}// namespace vox