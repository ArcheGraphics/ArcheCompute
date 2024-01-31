//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/command.h"
#include "rhi/argument.h"

namespace vox {
class Counter : public std::enable_shared_from_this<Counter> {
public:
    Counter() = default;
    virtual ~Counter() = default;

    virtual void update_start_times() = 0;

    virtual void update_final_times() = 0;

    virtual double calculate_elapsed_seconds_between(uint32_t begin, uint32_t end) = 0;

    std::shared_ptr<CounterSampleCommand> sample_counters_in_buffer(std::uintptr_t index);
};
}// namespace vox
