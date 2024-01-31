//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "dtypes.h"

namespace vox {
template<>
TypeToDtype<bool>::operator Dtype() {
    return bool_;
}

template<>
TypeToDtype<uint8_t>::operator Dtype() {
    return uint8;
}

template<>
TypeToDtype<uint16_t>::operator Dtype() {
    return uint16;
}

template<>
TypeToDtype<uint32_t>::operator Dtype() {
    return uint32;
}

template<>
TypeToDtype<uint64_t>::operator Dtype() {
    return uint64;
}

template<>
TypeToDtype<int8_t>::operator Dtype() {
    return int8;
}

template<>
TypeToDtype<int16_t>::operator Dtype() {
    return int16;
}

template<>
TypeToDtype<int32_t>::operator Dtype() {
    return int32;
}

template<>
TypeToDtype<int64_t>::operator Dtype() {
    return int64;
}

template<>
TypeToDtype<float16_t>::operator Dtype() {
    return float16;
}

template<>
TypeToDtype<float>::operator Dtype() {
    return float32;
}

template<>
TypeToDtype<simd::quatf>::operator Dtype() {
    return quat;
}

template<>
TypeToDtype<simd::float2>::operator Dtype() {
    return float2;
}

template<>
TypeToDtype<simd::float3>::operator Dtype() {
    return float3;
}

template<>
TypeToDtype<simd::float4>::operator Dtype() {
    return float4;
}

template<>
TypeToDtype<simd::float2x2>::operator Dtype() {
    return float2x2;
}

template<>
TypeToDtype<simd::float3x3>::operator Dtype() {
    return float3x3;
}

template<>
TypeToDtype<simd::float4x4>::operator Dtype() {
    return float4x4;
}

template<>
TypeToDtype<Transform>::operator Dtype() {
    return transform;
}

template<>
TypeToDtype<SpatialVector>::operator Dtype() {
    return spatial_vector;
}

template<>
TypeToDtype<SpatialMatrix>::operator Dtype() {
    return spatial_matrix;
}

}// namespace vox