//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <simd/simd.h>

namespace vox {
typedef __attribute__((__ext_vector_type__(6))) float SpatialVector;

struct SpatialMatrix{
    SpatialVector columns[6];

    SpatialMatrix &operator=(const float &other);
    SpatialMatrix &operator=(float &&other);
};

struct Transform {
    simd::float3 p{};
    simd::quatf q;

    Transform &operator=(const float &other);
    Transform &operator=(float &&other);
};

}// namespace vox
