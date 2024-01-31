//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {

struct Device {
    Device(int index = 0) : index(index){};
    int index;
};

const Device &default_device();

void set_default_device(const Device &d);

bool operator==(const Device &lhs, const Device &rhs);
bool operator!=(const Device &lhs, const Device &rhs);

}// namespace vox
