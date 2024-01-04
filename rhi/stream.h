//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <memory>

namespace vox {
class Stream {
public:
    virtual ~Stream() = default;

    virtual void submit() = 0;
};

}// namespace vox