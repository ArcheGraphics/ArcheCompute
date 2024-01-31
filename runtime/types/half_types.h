//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#ifdef __ARM_FEATURE_FP16_SCALAR_ARITHMETIC

#include <arm_fp16.h>
namespace mlx::core {
typedef __fp16 float16_t;
} // namespace mlx::core

#endif // __ARM_FEATURE_FP16_SCALAR_ARITHMETIC