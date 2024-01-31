//  Copyright (c) 2024 Feng Yang
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

    std::string name() override;

    std::string builtin_lib_name() override;

    std::unique_ptr<MetalDebugCaptureExt> debug_capture();

    std::shared_ptr<Buffer> create_buffer(size_t element_stride, size_t size) override;

    std::shared_ptr<Stream> create_stream() override;

    std::shared_ptr<Kernel> create_kernel_with_source(const std::string &code, const std::string &entry) override;

    std::shared_ptr<Kernel> create_kernel_from_library(const std::string &lib_name, const std::string &entry) override;

    void register_library(const std::string &lib_name) override;

    std::shared_ptr<Counter> create_counter(uint32_t count) override;

private:
    MTL::Device *_device{nullptr};
    std::unordered_map<std::string, MTL::Library *> library_map_;
};
}// namespace vox