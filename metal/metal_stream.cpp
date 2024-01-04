//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_stream.h"

namespace vox {
MetalStream::MetalStream(MTL::Device *device)
    : _queue{device->newCommandQueue()} {
    _queue->retain();
}

MetalStream::~MetalStream() {
    _queue->release();
}

void MetalStream::submit() {
}
}// namespace vox