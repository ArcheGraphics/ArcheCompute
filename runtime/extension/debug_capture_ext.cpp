//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "debug_capture_ext.h"
#include "stream.h"
#include "device.h"
#include <fmt/format.h>

namespace vox {
namespace detail {
template<typename Object>
[[nodiscard]] inline auto create_capture_scope(std::string_view label,
                                               const DebugCaptureOption &option,
                                               Object object) noexcept {
    auto desc = MTL::CaptureDescriptor::alloc()->init();
    switch (option.output) {
        case DebugCaptureOption::Output::DEVELOPER_TOOLS:
            desc->setDestination(MTL::CaptureDestinationDeveloperTools);
            break;
        case DebugCaptureOption::Output::GPU_TRACE_DOCUMENT:
            desc->setDestination(MTL::CaptureDestinationGPUTraceDocument);
            break;
    }
    if (!option.file_name.empty()) {
        auto file_name = NS::String::alloc()->init(
            const_cast<char *>(option.file_name.data()),
            option.file_name.size(),
            NS::UTF8StringEncoding, false);
        desc->setOutputURL(NS::URL::fileURLWithPath(file_name));
        file_name->release();
    } else if (desc->destination() == MTL::CaptureDestinationGPUTraceDocument) {
        auto name = label.empty() ? "metal.gputrace" : fmt::format("{}.gputrace", label);
        auto mtl_name = NS::String::alloc()->init(
            name.data(), name.size(), NS::UTF8StringEncoding, false);
        desc->setOutputURL(NS::URL::fileURLWithPath(mtl_name));
        mtl_name->release();
    }

    auto scope = MTL::CaptureManager::sharedCaptureManager()->newCaptureScope(object);
    if (!label.empty()) {
        auto mtl_label = NS::String::alloc()->init(
            const_cast<char *>(label.data()), label.size(),
            NS::UTF8StringEncoding, false);
        scope->setLabel(mtl_label);
        mtl_label->release();
    }
    desc->setCaptureObject(scope);
    return DebugCaptureScope(desc, scope);
}
}// namespace detail

DebugCaptureScope DebugCaptureExt::create_scope(
    std::string_view label, const DebugCaptureOption &option) {
    return detail::create_capture_scope(label, option, device().handle());
}

DebugCaptureScope DebugCaptureExt::create_scope(uint32_t stream, std::string_view label, const DebugCaptureOption &option) {
    return detail::create_capture_scope(label, option, vox::stream(stream).queue());
}

DebugCaptureScope::DebugCaptureScope(MTL::CaptureDescriptor *descriptor,
                                     MTL::CaptureScope *scope) noexcept
    : _scope{scope}, _descriptor{descriptor} {}

DebugCaptureScope::~DebugCaptureScope() {
    if (_scope) { _scope->endScope(); }
    _scope->release();
    _descriptor->release();
}

void DebugCaptureScope::mark_begin() const noexcept {
    _scope->beginScope();
}

void DebugCaptureScope::mark_end() const noexcept {
    _scope->endScope();
}

void DebugCaptureScope::start_debug_capture() const noexcept {
    auto manager = MTL::CaptureManager::sharedCaptureManager();
    NS::Error *error = nullptr;
    manager->startCapture(_descriptor, &error);
    if (error) {
        __builtin_printf("Failed to start debug capture: %s\n", error->localizedDescription()->utf8String());
    }
}

void DebugCaptureScope::stop_debug_capture() const noexcept {
    auto manager = MTL::CaptureManager::sharedCaptureManager();
    manager->stopCapture();
}

}// namespace vox