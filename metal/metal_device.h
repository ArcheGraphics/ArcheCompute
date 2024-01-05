//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/device.h"
#include <Metal/Metal.hpp>

namespace vox {
class MetalDebugCaptureExt;

class MetalDevice : public Device {
public:
    MetalDevice();

    ~MetalDevice() override;

    std::string_view name() override;

    std::unique_ptr<MetalDebugCaptureExt> debug_capture();

    std::shared_ptr<Buffer> create_buffer(size_t size) override;

    std::shared_ptr<Stream> create_stream() override;

    std::shared_ptr<Kernel> create_kernel(const std::string &code, const std::string &entry) override;

private:
    MTL::Device *_device{nullptr};
};
}// namespace vox