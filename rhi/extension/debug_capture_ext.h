//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "device_extension.h"
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
    virtual ~DebugCaptureScope() = default;

    virtual void mark_begin() const noexcept = 0;

    virtual void mark_end() const noexcept = 0;

    virtual void start_debug_capture() const noexcept = 0;

    virtual void stop_debug_capture() const noexcept = 0;

    template<typename F>
    void with(F &&f) const noexcept {
        mark_begin();
        std::invoke(std::forward<F>(f));
        mark_end();
    }
};

class DebugCaptureExt : public DeviceExtension {
public:
    static constexpr std::string_view name = "DebugCaptureExt";

    [[nodiscard]] virtual std::unique_ptr<DebugCaptureScope> create_scope(
        std::string_view label, const DebugCaptureOption &option = {}) const noexcept = 0;

    [[nodiscard]] virtual std::unique_ptr<DebugCaptureScope> create_scope(
        const std::shared_ptr<Stream> &stream_handle,
        std::string_view label, const DebugCaptureOption &option = {}) const noexcept = 0;
};

}// namespace vox