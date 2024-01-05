//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "device_extension.h"
#include <string_view>

namespace vox {
class DebugCaptureExt : public DeviceExtension {
public:
    static constexpr std::string_view name = "DebugCaptureExt";
};
}// namespace vox