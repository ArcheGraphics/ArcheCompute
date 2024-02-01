//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>
#include <string>

namespace vox {
class Stream;

struct DebugCaptureOption {
    enum struct Output {
        DEVELOPER_TOOLS,
        GPU_TRACE_DOCUMENT,
    };
    Output output{Output::DEVELOPER_TOOLS};
    std::string file_name;
};

class DebugCaptureScope {
public:
    DebugCaptureScope(MTL::CaptureDescriptor *descriptor,
                      MTL::CaptureScope *scope) noexcept;

    ~DebugCaptureScope();

    void mark_begin() const noexcept;

    void mark_end() const noexcept;

    void start_debug_capture() const noexcept;

    void stop_debug_capture() const noexcept;

private:
    MTL::CaptureScope *_scope;
    MTL::CaptureDescriptor *_descriptor;
};

class DebugCaptureExt {
public:
    [[nodiscard]] static DebugCaptureScope create_scope(
        std::string_view label, const DebugCaptureOption &option = {});

    [[nodiscard]] static DebugCaptureScope create_scope(
        uint32_t stream,
        std::string_view label, const DebugCaptureOption &option = {});
};
}// namespace vox