//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_kernel.h"

namespace vox {
MetalKernel::MetalKernel(MTL::Device *device) {
    _pso->retain();
}

MetalKernel::~MetalKernel() {
    _pso->release();
}
}// namespace vox