//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_buffer.h"

namespace vox {
MetalBuffer::MetalBuffer(MTL::Device *device, size_t element_stride, size_t size)
    : Buffer{element_stride, size},
      _buffer{device->newBuffer(size_bytes(), MTL::ResourceStorageModeShared |
                                                  MTL::ResourceHazardTrackingModeTracked)} {
    _buffer->retain();
}

MetalBuffer::~MetalBuffer() {
    _buffer->release();
}

uint64_t MetalBuffer::address() const noexcept {
    return _buffer->gpuAddress();
}

}// namespace vox