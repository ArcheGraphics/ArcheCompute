//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "device_types.h"

namespace vox {
template<int NT, typename Op = plus<int> >
struct CTAReduce {
    typedef typename Op::first_argument_type T;
    enum { Size = NT, Capacity = NT };
    struct Storage { T shared[Capacity]; };
};

template<int NT>
struct CTAReduce<NT, plus<int>> {
    typedef plus<int> Op;
    typedef int T;
    enum { Size = NT,
           Capacity = WARP_SIZE };
    struct Storage {
        int shared[Capacity];
    };
};

}// namespace vox
