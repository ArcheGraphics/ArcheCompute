//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "device.h"

namespace vox {

struct Stream {
    int index;
    Device device;
    explicit Stream(int index, Device device) : index(index), device(device) {}
};

inline bool operator==(const Stream &lhs, const Stream &rhs) {
    return lhs.index == rhs.index;
}

inline bool operator!=(const Stream &lhs, const Stream &rhs) {
    return !(lhs == rhs);
}

/** Get the default stream for the given device. */
Stream default_stream();

/** Make the stream the default for its device. */
void set_default_stream(Stream s);

/** Make a new stream on the given device. */
Stream new_stream(Device d);

}// namespace vox
