//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "counter.h"

namespace vox {
std::shared_ptr<CounterSampleCommand> Counter::sample_counters_in_buffer(std::uintptr_t index) {
    return std::make_shared<CounterSampleCommand>(shared_from_this(), index);
}

}// namespace vox