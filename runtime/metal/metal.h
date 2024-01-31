//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <future>
#include <memory>
#include <vector>

#include "../stream.h"

namespace vox::metal {

void new_stream(Stream stream);
std::shared_ptr<void> new_scoped_memory_pool();

}// namespace vox::metal
