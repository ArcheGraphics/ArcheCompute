//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/extension/debug_capture_ext.h"
#include <Metal/Metal.hpp>

namespace vox {
class MetalDebugCaptureScope : public DebugCaptureScope {
public:
    MetalDebugCaptureScope(MTL::CaptureDescriptor *descriptor,
                           MTL::CaptureScope *scope) noexcept;

    ~MetalDebugCaptureScope() override;

    void mark_begin() const noexcept override;

    void mark_end() const noexcept override;

    void start_debug_capture() const noexcept override;

    void stop_debug_capture() const noexcept override;

private:
    MTL::CaptureScope *_scope;
    MTL::CaptureDescriptor *_descriptor;
};

class MetalDebugCaptureExt : public DebugCaptureExt {
public:
    explicit MetalDebugCaptureExt(MTL::Device *device) noexcept;
    ~MetalDebugCaptureExt() noexcept override;

    [[nodiscard]] std::unique_ptr<DebugCaptureScope> create_scope(
        std::string_view label, const DebugCaptureOption &option = {}) const noexcept override;

    [[nodiscard]] std::unique_ptr<DebugCaptureScope> create_scope(
        const std::shared_ptr<Stream> &stream_handle,
        std::string_view label, const DebugCaptureOption &option = {}) const noexcept override;

private:
    MTL::Device *_device;
};
}// namespace vox