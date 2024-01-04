//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_device.h"
#include "metal_buffer.h"
#include "metal_stream.h"

namespace vox {
MetalDevice::MetalDevice()
    : _device{MTL::CreateSystemDefaultDevice()} {
    _device->retain();
}

MetalDevice::~MetalDevice() {
    _device->release();
}

std::string_view MetalDevice::name() {
    return "metal";
}

std::shared_ptr<Buffer> MetalDevice::create_buffer() {
    return std::make_shared<MetalBuffer>(_device);
}

std::shared_ptr<Stream> MetalDevice::create_stream() {
    return std::make_shared<MetalStream>(_device);
}

}// namespace vox