//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "device.h"

namespace vox {
class Counter {
public:
    explicit Counter(uint32_t sampleCount,
                     MTL::CommonCounterSet counterSetName = MTL::CommonCounterSetTimestamp);

    static bool is_counter_support(MTL::CounterSet *counterSet, MTL::CommonCounter counterName);

    static MTL::CounterSet *get_counter_set(MTL::CommonCounterSet counterSetName);

    static std::vector<MTL::CounterSamplingPoint> sampling_boundaries();

public:
    [[nodiscard]] MTL::CounterSampleBuffer *get_handle() const;

    void update_start_times();

    void update_final_times();

    double calculate_elapsed_seconds_between(uint32_t begin, uint32_t end);

    void sample_counters_in_buffer(std::uintptr_t index, uint32_t stream = 0) const;

private:
    [[nodiscard]] double absolute_time_in_microseconds(MTL::Timestamp timestamp) const;

    [[nodiscard]] double micro_seconds_between(MTL::Timestamp begin, MTL::Timestamp end) const;

private:
    MTL::CounterSampleBuffer *buffer;

    double cpuStart{};
    double gpuStart{};
    double cpuTimeSpan{};
    double gpuTimeSpan{};
};

}// namespace vox