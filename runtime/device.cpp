//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "device.h"

namespace vox {

static Device default_device_ = Device();

const Device &default_device() {
    return default_device_;
}

void set_default_device(const Device &d) {
    default_device_ = d;
}

bool operator==(const Device &lhs, const Device &rhs) {
    return lhs.index == rhs.index;
}

bool operator!=(const Device &lhs, const Device &rhs) {
    return !(lhs == rhs);
}

}// namespace vox
