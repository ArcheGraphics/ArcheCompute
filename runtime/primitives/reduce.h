//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "../array.h"

namespace vox {
void reduce(const Array &src, Array &dst, uint32_t stream = 0);

}// namespace vox