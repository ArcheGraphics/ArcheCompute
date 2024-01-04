//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/stream.h"

namespace vox {
class CUDAStream : public Stream {
public:
    CUDAStream();

    ~CUDAStream() override;
};
}// namespace vox