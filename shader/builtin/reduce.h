//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <metal_atomic>
#include <metal_simdgroup>

#include "atomic.h"
#include "utils.h"

union bool4_or_uint {
    bool4 b;
    unsigned int i;
};

struct None {
    template<typename T>
    void atomic_update(device mlx_atomic<T> *out, T val, uint offset = 0) {
        mlx_atomic_store_explicit(out, val, offset);
    }
};

//MARK: - And
struct And {
    bool simd_reduce(bool val) {
        return simd_all(val);
    };

    static constexpr constant bool init = true;

    void atomic_update(
        device mlx_atomic<unsigned int> *out,
        bool val,
        int elem_idx,
        int offset = 0) {
        if (!val) {
            bool4_or_uint update;
            update.b = {true, true, true, true};
            update.b[elem_idx] = false;
            mlx_atomic_fetch_and_explicit(out, update.i, offset);
        }
    }

    void atomic_update(device mlx_atomic<bool> *out, bool val, uint offset = 0) {
        if (!val) {
            mlx_atomic_store_explicit(out, val, offset);
        }
    }

    // Non atomic update
    void update(device bool *out, bool val) {
        *out &= val;
    }

    // Operator
    bool operator()(bool a, bool b) {
        return a && b;
    }
};

// MARK: - Or
struct Or {
    bool simd_reduce(bool val) {
        return simd_any(val);
    };

    static constexpr constant bool init = false;

    void atomic_update(
        device mlx_atomic<unsigned int> *out,
        bool val,
        uint elem_idx,
        uint offset = 0) {
        if (val) {
            bool4_or_uint update;
            update.b = {false, false, false, false};
            update.b[elem_idx] = true;
            mlx_atomic_fetch_or_explicit(out, update.i, offset);
        }
    }

    void atomic_update(device mlx_atomic<bool> *out, bool val, uint offset = 0) {
        if (val) {
            mlx_atomic_store_explicit(out, val, offset);
        }
    }

    // Non atomic update
    void update(device bool *out, bool val) {
        *out |= val;
    }

    // Operator
    bool operator()(bool a, bool b) {
        return a || b;
    }
};

//MARK: - Sum
template<typename U>
struct Sum {
    template<typename T>
    T simd_reduce(T val) {
        return simd_sum(val);
    };

    static constexpr constant U init = U(0);

    template<typename T>
    void atomic_update(device mlx_atomic<T> *out, T val, uint offset = 0) {
        mlx_atomic_fetch_add_explicit(out, val, offset);
    }

    // Operator
    U operator()(U a, U b) {
        return a + b;
    }
};

template<>
struct Sum<float2> {
    float2 simd_reduce(float2 val) {
        return simd_sum(val);
    };

    static constexpr constant float2 init = float2(0);

    void atomic_update(device mlx_atomic<float> *out, float2 val) {
        mlx_atomic_fetch_add_explicit(out, val[0], 0);
        mlx_atomic_fetch_add_explicit(out, val[1], 1);
    }

    // Operator
    float2 operator()(float2 a, float2 b) {
        return a + b;
    }
};

template<>
struct Sum<float4> {
    float4 simd_reduce(float4 val) {
        return simd_sum(val);
    };

    static constexpr constant float4 init = float4(0);

    void atomic_update(device mlx_atomic<float> *out, float4 val) {
        mlx_atomic_fetch_add_explicit(out, val[0], 0);
        mlx_atomic_fetch_add_explicit(out, val[1], 1);
        mlx_atomic_fetch_add_explicit(out, val[2], 2);
        mlx_atomic_fetch_add_explicit(out, val[3], 3);
    }

    // Operator
    float4 operator()(float4 a, float4 b) {
        return a + b;
    }
};

//MARK: - Prod
template<typename U>
struct Prod {
    template<typename T>
    T simd_reduce(T val) {
        return simd_product(val);
    };

    static constexpr constant U init = U(1);

    template<typename T>
    void atomic_update(device mlx_atomic<T> *out, T val, uint offset = 0) {
        mlx_atomic_fetch_mul_explicit(out, val, offset);
    }

    // Operator
    U operator()(U a, U b) {
        return a * b;
    }
};

template<>
struct Prod<float2> {
    float2 simd_reduce(float2 val) {
        return simd_product(val);
    };

    static constexpr constant float2 init = float2(1);

    void atomic_update(device mlx_atomic<float> *out, float2 val, uint offset = 0) {
        mlx_atomic_fetch_mul_explicit(out, val[0], 0);
        mlx_atomic_fetch_mul_explicit(out, val[1], 1);
    }

    // Operator
    float2 operator()(float2 a, float2 b) {
        return a * b;
    }
};

template<>
struct Prod<float4> {
    float4 simd_reduce(float4 val) {
        return simd_product(val);
    };

    static constexpr constant float4 init = float4(1);

    void atomic_update(device mlx_atomic<float> *out, float4 val, uint offset = 0) {
        mlx_atomic_fetch_mul_explicit(out, val[0], 0);
        mlx_atomic_fetch_mul_explicit(out, val[1], 1);
        mlx_atomic_fetch_mul_explicit(out, val[2], 2);
        mlx_atomic_fetch_mul_explicit(out, val[3], 3);
    }

    // Operator
    float4 operator()(float4 a, float4 b) {
        return a * b;
    }
};

//MARK: - Min
template<typename U>
struct Min {
    template<typename T>
    T simd_reduce(T val) {
        return simd_min(val);
    };

    static constexpr constant U init = Limits<U>::max;

    template<typename T>
    void atomic_update(device mlx_atomic<T> *out, T val, uint offset = 0) {
        mlx_atomic_fetch_min_explicit(out, val, offset);
    }

    // Operator
    U operator()(U a, U b) {
        return a < b ? a : b;
    }
};

// MARK: - Max
template<typename U>
struct Max {
    template<typename T>
    T simd_reduce(T val) {
        return simd_max(val);
    };

    static constexpr constant U init = Limits<U>::min;

    template<typename T>
    void atomic_update(device mlx_atomic<T> *out, T val, uint offset = 0) {
        mlx_atomic_fetch_max_explicit(out, val, offset);
    }

    // Operator
    U operator()(U a, U b) {
        return a > b ? a : b;
    }
};
