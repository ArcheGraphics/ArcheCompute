//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "types/spatial.h"
#include "types/half_types.h"

namespace vox {
struct Dtype {
    enum class Val {
        bool_,
        uint8,
        uint16,
        uint32,
        uint64,
        int8,
        int16,
        int32,
        int64,
        float16,
        float32,

        quat,
        float2,
        float3,
        float4,
        float2x2,
        float3x3,
        float4x4,

        transform,
        spatial_vector,
        spatial_matrix
    };

    Val val;
    const uint8_t size;
    constexpr explicit Dtype(Val val, uint8_t size) : val(val), size(size){};
    constexpr operator Val() const {
        return val;
    };
};

static constexpr Dtype bool_{Dtype::Val::bool_, sizeof(bool)};

static constexpr Dtype uint8{Dtype::Val::uint8, sizeof(uint8_t)};
static constexpr Dtype uint16{Dtype::Val::uint16, sizeof(uint16_t)};
static constexpr Dtype uint32{Dtype::Val::uint32, sizeof(uint32_t)};
static constexpr Dtype uint64{Dtype::Val::uint64, sizeof(uint64_t)};

static constexpr Dtype int8{Dtype::Val::int8, sizeof(int8_t)};
static constexpr Dtype int16{Dtype::Val::int16, sizeof(int16_t)};
static constexpr Dtype int32{Dtype::Val::int32, sizeof(int32_t)};
static constexpr Dtype int64{Dtype::Val::int64, sizeof(int64_t)};

static constexpr Dtype float16{Dtype::Val::float16, sizeof(uint16_t)};
static constexpr Dtype float32{Dtype::Val::float32, sizeof(float)};

static constexpr Dtype quat{Dtype::Val::quat, sizeof(simd::quatf)};
static constexpr Dtype float2{Dtype::Val::float2, sizeof(simd::float2)};
static constexpr Dtype float3{Dtype::Val::float3, sizeof(simd::float3)};
static constexpr Dtype float4{Dtype::Val::float4, sizeof(simd::float4)};
static constexpr Dtype float2x2{Dtype::Val::float2x2, sizeof(simd::float2x2)};
static constexpr Dtype float3x3{Dtype::Val::float3x3, sizeof(simd::float3x3)};
static constexpr Dtype float4x4{Dtype::Val::float4x4, sizeof(simd::float4x4)};

static constexpr Dtype transform{Dtype::Val::transform, sizeof(Transform)};
static constexpr Dtype spatial_vector{Dtype::Val::spatial_vector, sizeof(SpatialVector)};
static constexpr Dtype spatial_matrix{Dtype::Val::spatial_matrix, sizeof(SpatialMatrix)};

inline uint8_t size_of(const Dtype &t) {
    return t.size;
}

template<typename T>
struct TypeToDtype {
    operator Dtype();
};
}// namespace vox