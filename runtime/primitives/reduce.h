//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "../array.h"

namespace vox {
enum class ReduceType { And, Or, Sum, Prod, Min, Max };

void reduce(const Array &src, Array &dst, ReduceType reduce_type, uint32_t stream = 0);

}// namespace vox