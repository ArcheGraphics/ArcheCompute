//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

namespace vox {
class Buffer;

class Device {
public:
    virtual std::string_view name() = 0;

    virtual std::shared_ptr<Buffer> create_buffer() = 0;
};

}// namespace vox