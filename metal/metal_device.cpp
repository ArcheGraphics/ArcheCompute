//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_device.h"
#include "metal_buffer.h"

namespace vox {
std::string_view MetalDevice::name() {
    return "";
}

std::shared_ptr<Buffer> MetalDevice::create_buffer() {
    return std::make_shared<MetalBuffer>();
}

}// namespace vox