//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/stream.h"
#include <Metal/Metal.hpp>

namespace vox {
class MetalStream : public Stream {
public:
    MetalStream(MTL::Device *device);

    ~MetalStream() override;

    void submit() override;

private:
    MTL::CommandQueue *_queue{nullptr};
};
}// namespace vox