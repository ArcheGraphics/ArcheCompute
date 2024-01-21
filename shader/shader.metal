//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;

#include "array.h"
#include "vec.h"
#include "mat.h"
#include "quat.h"
#include "spatial.h"

#include "rand.h"
#include "noise.h"

#include "hash_grid.h"

void test(device array_t<atomic_int>& arr) {
    atomic_add(arr, 1, 1);
    atomic_sub(arr, 1, 1);
    atomic_min(arr, 1, 1);
    atomic_max(arr, 1, 1);
}
