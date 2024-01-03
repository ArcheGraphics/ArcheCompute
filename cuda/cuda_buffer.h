//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "core/buffer.h"
#include <Metal/Metal.hpp>

namespace vox {
class CUDABuffer : public Buffer {
public:
    CUDABuffer();

    ~CUDABuffer() override;
};
}// namespace vox