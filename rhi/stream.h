//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include "argument.h"
#include "kernel.h"

namespace vox {
class Stream {
public:
    virtual ~Stream() = default;

    virtual void synchronize() = 0;

    virtual void dispatch(const std::vector<std::shared_ptr<Command>> &commands) = 0;
};

}// namespace vox