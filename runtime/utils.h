//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "array.h"
#include "device.h"
#include "dtypes.h"
#include "stream.h"

namespace vox {

struct PrintFormatter {
    inline static void print(std::ostream &os, bool val);
    inline static void print(std::ostream &os, int16_t val);
    inline static void print(std::ostream &os, uint16_t val);
    inline static void print(std::ostream &os, int32_t val);
    inline static void print(std::ostream &os, uint32_t val);
    inline static void print(std::ostream &os, int64_t val);
    inline static void print(std::ostream &os, uint64_t val);
    inline static void print(std::ostream &os, float16_t val);
    inline static void print(std::ostream &os, float val);
    inline static void print(std::ostream &os, simd::quatf val);
    inline static void print(std::ostream &os, simd::float2 val);
    inline static void print(std::ostream &os, simd::float3 val);
    inline static void print(std::ostream &os, simd::float4 val);
    inline static void print(std::ostream &os, simd::float2x2 val);
    inline static void print(std::ostream &os, simd::float3x3 val);
    inline static void print(std::ostream &os, simd::float4x4 val);
    inline static void print(std::ostream &os, Transform val);
    inline static void print(std::ostream &os, SpatialVector val);
    inline static void print(std::ostream &os, SpatialMatrix val);

    static bool capitalize_bool;
};

std::string type_to_name(const Dtype &d);

std::ostream &operator<<(std::ostream &os, const Dtype &d);
std::ostream &operator<<(std::ostream &os, const std::vector<int> &v);
std::ostream &operator<<(std::ostream &os, const std::vector<size_t> &v);

MTL::Size get_block_dims(int dim0, int dim1, int dim2);

}// namespace vox
