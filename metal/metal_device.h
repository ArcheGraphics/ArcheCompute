//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "core/device.h"

namespace vox {
class MetalDevice : public Device {
public:
    std::string_view name() override;

    std::shared_ptr<Buffer> create_buffer() override;
};
}// namespace vox