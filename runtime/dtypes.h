//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <simd/simd.h>
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

inline uint8_t size_of(const Dtype& t) {
    return t.size;
}

template <typename T>
struct TypeToDtype {
    operator Dtype();
};
}// namespace vox