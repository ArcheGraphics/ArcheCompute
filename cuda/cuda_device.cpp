//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cuda_device.h"
#include "cuda_buffer.h"

namespace vox {
CUDADevice::CUDADevice() {
}

CUDADevice::~CUDADevice() {
}

std::string_view CUDADevice::name() {
    return "cuda";
}

std::shared_ptr<Buffer> CUDADevice::create_buffer() {
    return std::make_shared<CUDABuffer>();
}

}// namespace vox