//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <simd/simd.h>

namespace vox {
typedef __attribute__((__ext_vector_type__(6))) float SpatialVector;

typedef struct {
    SpatialVector columns[6];
} SpatialMatrix;

struct Transform {
    simd::float3 p{};
    simd::quatf q;
};

}// namespace vox
